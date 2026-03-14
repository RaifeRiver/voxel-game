use crate::block::block::{BasicBlock, Block};
use crate::registry::registry::{Registry, RegistryID};
use std::sync::OnceLock;
use crate::basic_block;
use crate::block::block_properties::BlockProperties;

pub struct BlockIDs {
	pub air: u32,
	pub dirt: u32,
	pub grass: u32,
	pub stone: u32,
	pub snow: u32,
}

pub static IDS: OnceLock<BlockIDs> = OnceLock::new();

pub type BlockRegistry = Registry<Box<dyn Block>>;

pub static BLOCKS: OnceLock<BlockRegistry> = OnceLock::new();

macro_rules! id {
	( $x:expr ) => {
		RegistryID::new("voxel_game", $x)
	}
}

pub fn init() {
	let mut registry: BlockRegistry = Registry::new();

	let ids = BlockIDs {
		air: registry.register(id!("air"), basic_block!(hardness(0.0))),
		dirt: registry.register(id!("dirt"), basic_block!(hardness(1.0).colour_rgb(87, 48, 8))),
		grass: registry.register(id!("grass"), basic_block!(hardness(1.5).colour_rgb(20, 102, 13))),
		stone: registry.register(id!("stone"), basic_block!(hardness(4.0).colour_rgb(92, 99, 102))),
		snow: registry.register(id!("snow"), basic_block!(hardness(0.5).colour_rgb(242, 242, 242))),
	};

	registry.lock();

	BLOCKS.set(registry).ok();
	IDS.set(ids).ok();
}