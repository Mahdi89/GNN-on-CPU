
/*
 * Direct map histogram calculation example using VAQUERO
 * instructions. We implement the algorithm in C++ using
 * VAQUERO instrinsics for the aarch64 SVE instructions.
 * We assume an 8-bit histogram. Additionally, operations as re-
 * ductions are simplified (similar to pseudocode).
 * Terminology:
 * ASPM: Advanced Scratchpad Memory
 *
 * List of VAQUERO intrinsics used:
 * 1. clear_aspm : clear the content in the ASPM.
 * 2. svVaqAdd_dir_aspm_u32 : 32-bit Vaquero addition in direct mode
 *                            and store the result in the ASPM.
 * 3. aspm_load_dir_u32 : load 32-bit elements in direct mode from
 *                          the ASPM into a vector register.
 */

uint32_t * histogram_calc(uint32_t * input_values,
      uint32_t input_size) {
   // First allocate the histogram in memory
   uint32_t * histogram = (uint32_t *)calloc(256, sizeof(uint32_t));

   // Vector variables
   uint64_t vlen = svcntb();  // Vector length for byte data type
   svuint32_t idx_v;          // Loaded indices
   svuint32_t values_v;       // Reduced values
   svbool_t red_m;            // Mask for the reduced values

   // First clear the ASPM
   clear_aspm();

   // Main execution loop
   for (uint32_t it=0; it<input_size; it+=vlen) {
      // execution mask for byte data type
      svbool_t it_m = svwhilelt_b8_u32(it, input_size);
      // load the input into a vector register
      idx_v = svld1_u32(it_m, (input_values + it));
      // Extract the selected 8 bits
      idx_v = svand_u32(it_m, idx_v, 0x000000ff);
      // Reduce repeated values in the idx_v register
      values_v = REDUCE(idx_v);
      // Get the mask for the reduced values
      red_m = GET_MASK(idx_v);
      // Execute a VAQUERO Addition that uses the idx_v register
      // as the indices to read the ASPM. Then, the result is
      // stored back to the ASPM using the same indices in idx_v
      // For control we use the reduced values mask red_m.
      // intruction:
      // svVaqAdd_dir_aspm_u32(mask, indices, values)
      svVaqAdd_dir_aspm_u32(red_m, idx_v, values_v);
   }

   // When the previous loop finishes traversing the input values
   // the algorithm reads the histogram in the ASPM and stores it
   // back to memory
   for (uint32_t it=0; it<256; it+=vlen) {
      // Create the execution mask
      svbool_t it_m = svwhilelt_b32_u32(it, 256);
      // Create the indices to read the data in the ASPM
      // Indices starting in it and increase by 1.
      idx_v = svindex_u32(it, 1);
      // read the histogram from VAQUERO
      // Instruction:
      // svload_aspm_dir_u32(mask, indices);
      values_v = aspm_load_dir_u32(it_m, idx_v);
      // Store the data in the memory
      svst1_u32(it_m, &hist[it], values_v);
   }
}

/*
 * Associative mode histogram calculation example using VAQUERO
 * instructions. We implement the algorithm in C++ using VAQUERO
 * instrinsics for the aarch64 SVE instructions.
 * We assume a 16-bit histogram that does not fit in the ASPM.
 * Additionally, operations as reductions are simplified 
 * (similar to pseudocode).
 * Terminology:
 * ASPM : Advanced Scratchpad Memory
 * ASPM.key_table.size : Architectural parameter that specifies
 *                       the size (in elements) of the key table
 *                       in the ASPM.
 *
 * List of VAQUERO intrinsics used:
 * 1. clear_aspm : clear the content in the ASPM.
 * 2. svVaqAdd_asso_aspm_u32 : 32-bit Vaquero addition in associative
 *                             mode and store the result in the ASPM.
 * 3. aspm_buffer_size : read the number of elements stored in the
 *                       replacement buffer.
 * 4. aspm_read_buffer : Read the content of the replacement buffer.
 * 5. aspm_read_table : Read the keys stored in the key table.
 * 6. aspm_load_asso_u32 : load 32-bit elements in associative mode
 *                          from the ASPM into a vector register.
 */

uint32_t * histogram_calc(uint32_t * input_values,
      uint32_t input_size) {
   // First allocate the histogram in memory
   uint32_t * histogram = (uint32_t *)calloc(65536, sizeof(uint32_t));

   // Vector variables
   uint64_t vlen = svcntb();  // Vector length for byte data type
   svuint32_t idx_v;          // Loaded indices
   svuint32_t values_v;       // Reduced values
   svbool_t red_m;            // Mask for the reduced values

   // First clear the ASPM
   clear_aspm();

   // Main execution loop
   for (uint32_t it=0; it<input_size; it+=vlen) {
      // execution mask for byte data type
      svbool_t it_m = svwhilelt_b8_u32(it, input_size);
      // load the input into a vector register
      idx_v = svld1_u32(it_m, (input_values + it));
      // Extract the selected 16 bits
      idx_v = svand_u32(it_m, idx_v, 0x0000ffff);
      // Reduce repeated values in the idx_v register
      values_v = REDUCE(idx_v);
      // Get the mask for the reduced values
      red_m = GET_MASK(idx_v);
      // Execute a VAQUERO Addition that uses the idx_v register
      // as the indices to read the ASPM. Then, the result is
      // stored back to the ASPM using the same indices in idx_v
      // For control we use the reduced values mask red_m.
      // intruction:
      // svVaqAdd_asso_aspm_u32(mask, indices, values)
      svVaqAdd_asso_aspm_u32(red_m, idx_v, values_v);

      // Check if the replacement buffer is not empty
      if (aspm_buffer_size() > 0) {
         // Read the evicted indices and values
         // aspm_read_buffer : 0->index buffer, 1->values buffer
         svuint32_t repl_idx_v = aspm_read_buffer(0);
         svuint32_t repl_val_v = aspm_read_buffer(1);
         // Gather the old values in the histogram
         svuint32_t hist_v = svld1_gather_u32(histogram, repl_idx_v);
         // Update the old values
         hist_v = svadd_u32(hist_v, repl_val_v);
         // Scatter the updated values
         svst1_scatter_u32(histogram, repl_idx_v, repl_val_v);
      }
   }

   // When the previous loop finishes traversing the input values
   // the algorithm reads the histogram in the ASPM and stores it
   // back to memory
   for (uint32_t it=0; it<ASPM.key_table.size; it+=vlen) {
      // Create the execution mask
      svbool_t it_m = svwhilelt_b32_u32(it, ASPM.key_table.size);
      // Read the keys stored in the key table in VAQUERO
      // instruction:
      // aspm_read_table(key table offset offset)
      idx_v = aspm_read_table(it);
      // read the histogram from VAQUERO
      // Instruction:
      // svload_aspm_asso_u32(mask, indices);
      values_v = aspm_load_asso_u32(it_m, idx_v);
      // Gather the old values in the histogram
      svuint32_t hist_v = svld1_gather_u32(histogram, idx_v);
      // Update the old values
      hist_v = svadd_u32(hist_v, values_v);
      // Scatter the updated values
      svst1_scatter_u32(histogram, idx_v, values_v);

   }
}
