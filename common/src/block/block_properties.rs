use glam::U8Vec4;

pub struct BlockProperties {
	hardness: f32,
	colour: U8Vec4,
}

impl BlockProperties {
	pub fn get_hardness(&self) -> f32 {
		self.hardness
	}
	
	pub fn get_colour(&self) -> U8Vec4 {
		self.colour
	}

	pub fn builder() -> BlockPropertiesBuilder {
		BlockPropertiesBuilder {
			hardness: 0.0,
			colour: U8Vec4::new(0, 0, 0, 0),
		}
	}
}

pub struct BlockPropertiesBuilder {
	hardness: f32,
	colour: U8Vec4,
}

impl BlockPropertiesBuilder {
	pub fn hardness(mut self, hardness: f32) -> Self {
		self.hardness = hardness;
		self
	}

	pub fn colour_rgb(mut self, r: u8, g: u8, b: u8) -> Self {
		self.colour.x = r;
		self.colour.y = g;
		self.colour.z = b;
		self.colour.w = 255;
		self
	}

	pub fn colour_rgba(mut self, r: u8, g: u8, b: u8, a: u8) -> Self {
		self.colour.x = r;
		self.colour.y = g;
		self.colour.z = b;
		self.colour.w = a;
		self
	}
	
	pub fn colour_vec(mut self, colour: U8Vec4) -> Self {
		self.colour = colour;
		self
	}

	pub fn build(self) -> BlockProperties {
		BlockProperties {
			hardness: self.hardness,
			colour: self.colour,
		}
	}
}