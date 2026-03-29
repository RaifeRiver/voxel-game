#include "RenderEngine.h"

#include <memory>

#include "VulkanImage.h"
#include "VulkanInitialisers.h"
#include "VulkanUtil.h"
#include "client/VoxelGameClient.h"
#include "glm/vec3.hpp"

namespace voxel_game::client::renderer {
	RenderEngine::RenderEngine() {
		mWindowManager = std::make_unique<WindowManager>();
		mWindowManager->showWindow();

		mRenderContext = std::make_unique<RenderContext>(mWindowManager.get());
	}

	void RenderEngine::render() {
		WindowManager::pollEvents();

		if (needsResize) {
			mRenderContext->resizeSwapchain(mWindowManager.get());
			needsResize = false;
		}

		VK_CHECK(vkWaitForFences(mRenderContext->getDevice(), 1, &mRenderContext->getCurrentFrameContext().getRenderFence(), true, UINT64_MAX));
		VK_CHECK(vkResetFences(mRenderContext->getDevice(), 1, &mRenderContext->getCurrentFrameContext().getRenderFence()));

		uint32_t swapchainIndex;
		if (vkAcquireNextImageKHR(mRenderContext->getDevice(), mRenderContext->getSwapchain(), UINT64_MAX, mRenderContext->getCurrentFrameContext().getSwapchainSemaphore(), nullptr, &swapchainIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
			needsResize = true;
			return;
		}

		const VkCommandBuffer commandBuffer = mRenderContext->getCurrentFrameContext().getCommandBuffer();
		VK_CHECK(vkResetCommandBuffer(commandBuffer, 0));
		const VkCommandBufferBeginInfo commandBufferBeginInfo = initialisers::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		image::transitionImage(commandBuffer, mRenderContext->getDrawImage().image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		VoxelGameClient::getClient()->getUniverseRenderer()->render(mRenderContext.get(), commandBuffer);

		image::transitionImage(commandBuffer, mRenderContext->getDrawImage().image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		image::transitionImage(commandBuffer, mRenderContext->getSwapchainImages()[swapchainIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		const VkExtent2D drawImageExtent = {mRenderContext->getDrawImage().extent.width, mRenderContext->getDrawImage().extent.height};
		image::copyImageToImage(commandBuffer, mRenderContext->getDrawImage().image, mRenderContext->getSwapchainImages()[swapchainIndex], drawImageExtent, mRenderContext->getSwapchainExtent());

		image::transitionImage(commandBuffer, mRenderContext->getSwapchainImages()[swapchainIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		TracyVkCollect(mRenderContext->getTracyContext(), commandBuffer);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		const VkCommandBufferSubmitInfo commandBufferSubmitInfo = initialisers::commandBufferSubmitInfo(commandBuffer);
		const VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo = initialisers::semaphoreSubmitInfo(mRenderContext->getCurrentFrameContext().getSwapchainSemaphore(), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR);
		const VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo = initialisers::semaphoreSubmitInfo(mRenderContext->getRenderSemaphores()[swapchainIndex], VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT);
		const VkSubmitInfo2 submitInfo = initialisers::submitInfo(&commandBufferSubmitInfo, &signalSemaphoreSubmitInfo, &waitSemaphoreSubmitInfo);
		VK_CHECK(vkQueueSubmit2(mRenderContext->getGraphicsQueue(), 1, &submitInfo, mRenderContext->getCurrentFrameContext().getRenderFence()));

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pSwapchains = &mRenderContext->getSwapchain();
		presentInfo.swapchainCount = 1;
		presentInfo.pWaitSemaphores = &mRenderContext->getRenderSemaphores()[swapchainIndex];
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pImageIndices = &swapchainIndex;
		if (vkQueuePresentKHR(mRenderContext->getGraphicsQueue(), &presentInfo) == VK_ERROR_OUT_OF_DATE_KHR) {
			needsResize = true;
		}

		mRenderContext->nextFrame();
	}

	void RenderEngine::destroy() const {
		mRenderContext->destroy();

		mWindowManager->destroy();
	}

	void RenderEngine::renderBackground(VkCommandBuffer commandBuffer) const {
		const float h = mRenderContext->getCurrentFrame() % 1000 / 1000.0f * 6.0f;
		const int i = static_cast<int>(h);
		const float f = h - i;

		const float q = 1.0f - f;
		const float t = f;

		glm::vec3 clearColour;
		switch (i % 6) {
			case 0:
				clearColour = {1.0f, t, 0.0f};
				break;
			case 1:
				clearColour = {q, 1.0f, 0.0f};
				break;
			case 2:
				clearColour = {0.0f, 1.0f, t};
				break;
			case 3:
				clearColour = {0.0f, q, 1.0f};
				break;
			case 4:
				clearColour = {t, 0.0f, 1.0f};
				break;
			case 5:
				clearColour = {1.0f, 0.0f, q};
				break;
			default:
				clearColour = {0.0f, 0.0f, 0.0f};
				break;
		}
		const VkClearColorValue clearValue = {clearColour.r, clearColour.g, clearColour.b, 1.0f};
		const VkImageSubresourceRange clearRange = initialisers::imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
		vkCmdClearColorImage(commandBuffer, mRenderContext->getDrawImage().image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
	}
}
