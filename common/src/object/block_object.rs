use std::collections::HashMap;
use glam::IVec3;
use crate::object::chunk::Chunk;
use crate::object::physics_object::{PhysicsObject, PhysicsObjectData};

struct BlockObject {
	physics_data: PhysicsObjectData,
	blocks: HashMap<IVec3, Chunk>,
}

impl PhysicsObject for BlockObject {
	fn get_data(&mut self) -> &mut PhysicsObjectData {
		&mut self.physics_data
	}
}