use glam::{DVec3, I64Vec3, Quat, Vec3};
use slotmap::new_key_type;

pub const SECTOR_SIZE: i32 = 64;
pub const SECTOR_SIZE_FLOAT: f64 = SECTOR_SIZE as f64;

pub const DRAG: f32 = 0.95;

pub struct PhysicsObjectData {
	pub sector_pos: I64Vec3,
	pub local_pos: Vec3,
	pub rotation: Quat,
	pub velocity: Vec3,
	pub id: u64,
}

pub trait PhysicsObject {
	fn get_data(&mut self) -> &mut PhysicsObjectData;

	fn relative_teleport(&mut self, distance: DVec3) {
		let data = self.get_data();
		for i in 0..3 {
			let sectors = (distance[i] / SECTOR_SIZE_FLOAT).floor();
			data.sector_pos[i] += sectors as i64;
			data.local_pos[i] += distance[i].rem_euclid(SECTOR_SIZE_FLOAT) as f32;
		}
	}

	fn teleport(&mut self, pos: DVec3) {
		let data = self.get_data();
		for i in 0..3 {
			data.sector_pos[i] = (pos[i] / SECTOR_SIZE_FLOAT).floor() as i64;
			data.local_pos[i] = pos[i].rem_euclid(SECTOR_SIZE_FLOAT) as f32;
		}
	}

	fn apply_force(&mut self, force: Vec3, delta_time: f32) {
		self.get_data().velocity += force * delta_time;
	}

	fn apply_relative_force(&mut self, force: Vec3, delta_time: f32) {
		let rotation = self.get_data().rotation;
		self.get_data().velocity += rotation * force * delta_time;
	}

	fn apply_impulse(&mut self, impulse: Vec3) {
		self.get_data().velocity += impulse;
	}

	fn apply_relative_impulse(&mut self, impulse: Vec3) {
		let rotation = self.get_data().rotation;
		self.get_data().velocity += rotation * impulse;
	}

	fn update(&mut self, delta_time: f32) {
		let distance = self.get_data().velocity * delta_time;
		self.relative_teleport(DVec3::from(distance));
		self.get_data().velocity *= DRAG.powf(delta_time);
	}

	fn get_relative_pos(&mut self, other: &mut dyn PhysicsObject) -> DVec3 {
		let relative_sector = (self.get_data().sector_pos - other.get_data().sector_pos) * SECTOR_SIZE as i64;
		let relative_local_pos = self.get_data().local_pos - other.get_data().local_pos;
		DVec3::new(relative_sector.x as f64 + relative_local_pos.x as f64, relative_sector.y as f64 + relative_local_pos.y as f64, relative_sector.z as f64 + relative_local_pos.z as f64)
	}
}

new_key_type! {
	pub struct ObjectID;
}