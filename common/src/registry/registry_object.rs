pub struct RegistryObjectData {
	numeric_id: u32,
	id: String,
}

impl RegistryObjectData {
	pub fn new() -> Self {
		Self {
			numeric_id: 0,
			id: String::new(),
		}
	}
}

pub trait RegistryObject {
	fn get_data(&mut self) -> &mut RegistryObjectData;

	fn get_numeric_id(&mut self) -> u32 {
		self.get_data().numeric_id
	}

	fn get_id(&mut self) -> &str {
		&self.get_data().id
	}

	fn register(&mut self, numeric_id: u32, id: String) {
		self.get_data().numeric_id = numeric_id;
		self.get_data().id = id;
	}
}

impl<T: ?Sized + RegistryObject> RegistryObject for Box<T> {
	fn get_data(&mut self) -> &mut RegistryObjectData {
		self.as_mut().get_data()
	}
}