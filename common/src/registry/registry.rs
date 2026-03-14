use crate::registry::registry_object::RegistryObject;
use std::collections::HashMap;
use std::fmt::{Display, Formatter};

#[derive(Eq, Hash, PartialEq)]
pub struct RegistryID {
	namespace: String,
	id: String,
}
impl RegistryID {
	pub fn new(namespace: &str, id: &str) -> Self {
		Self {
			namespace: namespace.to_string(),
			id: id.to_string(),
		}
	}

	pub fn get_full_id(&self) -> String {
		format!("{}:{}", self.namespace, self.id)
	}
}

impl Display for RegistryID {
	fn fmt(&self, formatter: &mut Formatter<'_>) -> std::fmt::Result {
		write!(formatter, "{}:{}", self.namespace, self.id)
	}
}

pub struct Registry<T: RegistryObject> {
	objects: Vec<Box<T>>,
	ids: HashMap<RegistryID, u32>,
	locked: bool,
}

impl<T: RegistryObject> Registry<T> {
	pub fn new() -> Self {
		Self {
			objects: Vec::new(),
			ids: HashMap::new(),
			locked: false,
		}
	}
	
	pub fn register(&mut self, id: RegistryID, object: T) -> u32 {
		if self.locked {
			panic!("Registry is locked");
		}
		if self.ids.contains_key(&id) {
			panic!("Registry already contains object with id {}", id);
		}
		let numeric_id = self.objects.len() as u32;
		self.objects.push(Box::new(object));
		self.ids.insert(id, numeric_id);
		numeric_id
	}

	pub fn get_numeric_id(&self, id: &RegistryID) -> u32 {
		match self.ids.get(&id) {
			Some(numeric_id) => *numeric_id,
			None => panic!("Registry does not contain object with id {}", id),
		}
	}

	pub fn get_object_by_id(&self, id: &RegistryID) -> &T {
		let numeric_id = self.get_numeric_id(id);
		&self.objects[numeric_id as usize]
	}

	pub fn get_object(&self, id: u32) -> &T {
		match self.objects.get(id as usize) {
			Some(object) => &object,
			None => panic!("Registry does not contain object with numeric id {}", id),
		}
	}

	pub fn lock(&mut self) {
		if self.locked {
			panic!("Registry is already locked");
		}
		self.locked = true;
	}
}