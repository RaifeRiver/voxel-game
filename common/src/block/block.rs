use crate::block::block_properties::BlockProperties;
use crate::registry::registry_object::{RegistryObject, RegistryObjectData};

pub trait Block: RegistryObject + Send + Sync {
	fn get_block_properties(&self) -> &BlockProperties;
}

pub struct BasicBlock {
	registry_data: RegistryObjectData,
	properties: BlockProperties,
}

impl BasicBlock {
	pub fn new(properties: BlockProperties) -> Self {
		Self {
			registry_data: RegistryObjectData::new(),
			properties,
		}
	}
}

impl RegistryObject for BasicBlock {
	fn get_data(&mut self) -> &mut RegistryObjectData {
		&mut self.registry_data
	}
}

impl Block for BasicBlock {
	fn get_block_properties(&self) -> &BlockProperties {
		&self.properties
	}
}

#[macro_export]
macro_rules! basic_block {
	($($x:tt)*) => {
		Box::new(BasicBlock::new(BlockProperties::builder().$($x)*.build()))
	}
}