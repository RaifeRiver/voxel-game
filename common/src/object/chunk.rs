pub struct Chunk {
	blocks: Vec<u64>,
	block_types: Vec<(u32, i32)>,
	bits_per_block: u8,
	block_index_shift: u8,
	block_mask: u32,
	clear_mask: u64,
}

impl Chunk {
	pub fn get_block(&self, x: u32, y: u32, z: u32) -> u32 {
		self.get_block_(x & 31 | (y & 31) << 5 | (z & 31) << 10)
	}

	pub fn set_block(&mut self, x: u32, y: u32, z: u32, block: u32) {
		if self.bits_per_block == 0 && block == self.block_types[0].0 {
			return;
		}
		let pos = x & 31 | (y & 31) << 5 | (z & 31) << 10;
		let current_block = self.get_block_(pos);
		if block == current_block {
			return;
		}
		self.get_block_id_and_modify_count(current_block, -1);
		let id = self.get_block_id_and_modify_count(block, 1);
		let offset = (pos & self.block_mask) * self.bits_per_block as u32;
		self.blocks[(pos >> self.block_index_shift) as usize] &= !(self.clear_mask << offset);
		self.blocks[(pos >> self.block_index_shift) as usize] |= (id as u64) << offset;
	}

	fn get_block_(&self, pos: u32) -> u32 {
		if self.bits_per_block == 0 {
			return self.block_types[0].0
		}
		
		self.block_types[(self.blocks[(pos >> self.block_index_shift) as usize] >> ((pos & self.block_mask) * self.bits_per_block as u32) & self.clear_mask) as usize].0
	}

	fn get_block_id_and_modify_count(&mut self, block: u32, count_modifier: i32) -> u32 {
		for i in 0..self.block_types.len() {
			if self.block_types[i].0 == block {
				self.block_types[i].1 += count_modifier;
				if self.block_types[i].1 == 0 && i != 0 {
					let last = self.block_types.len() - 1;
					self.block_types.swap_remove(i);
					for pos in 0..32768 {
						if self.get_block_(pos) == last as u32 {
							let offset = (pos & self.block_mask) * self.bits_per_block as u32;
							self.blocks[(pos >> self.block_index_shift) as usize] &= !(self.clear_mask << offset);
							self.blocks[(pos >> self.block_index_shift) as usize] |= (i as u64) << offset;
						}
					}
					if self.block_types.len() == 1 {
						self.blocks.clear();
						self.bits_per_block = 0;
					}
					else if self.blocks.len() < (1 << (self.bits_per_block >> 1)) {
						self.repack(self.bits_per_block >> 1);
					}
				}
				return i as u32
			}
		}

		let id = self.block_types.len();
		if id >= 1 << self.bits_per_block {
			self.repack(if self.bits_per_block == 0 {1} else {self.bits_per_block << 1});
		}
		self.block_types.push((block, count_modifier));
		id as u32
	}

	fn repack(&mut self, bits_per_block: u8) {
		let blocks_per_value = 64 / bits_per_block;
		let new_block_index_shift = blocks_per_value.trailing_zeros() as u8;
		let new_block_mask = blocks_per_value as u32 - 1;
		let new_clear_mask = (1 << bits_per_block) - 1;

		let old_blocks = std::mem::take(&mut self.blocks);

		self.blocks = vec!(0; 512 * bits_per_block as usize);

		if self.bits_per_block > 0 {
			for i in 0..32768 {
				let b = old_blocks[(i >> self.block_index_shift) as usize] >> ((i & self.block_mask) * bits_per_block as u32) & self.clear_mask;
				if b == 0 {
					continue;
				}
				let offset = (i & new_block_mask) * bits_per_block as u32;
				self.blocks[(i >> new_block_index_shift) as usize] |= b << offset;
			}
		}
		else {
			for i in 0..32768 {
				let offset = (i & new_block_mask) * bits_per_block as u32;
				self.blocks[(i >> new_block_index_shift) as usize] |= (self.block_types[0].0 as u64) << offset;
			}
		}

		self.bits_per_block = bits_per_block;
		self.block_index_shift = new_block_index_shift;
		self.block_mask = new_block_mask;
		self.clear_mask = new_clear_mask;
	}
}