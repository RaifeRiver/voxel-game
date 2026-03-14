use std::collections::HashMap;
use glam::I64Vec3;
use slotmap::SlotMap;
use crate::object::physics_object::{ObjectID, PhysicsObject};

pub struct Universe {
	object_ids: HashMap<I64Vec3, Vec<ObjectID>>,
	objects: SlotMap<ObjectID, Box<dyn PhysicsObject>>,
	next_id: u64,
}