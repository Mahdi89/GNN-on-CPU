/*
 * Data aggregation example using the VAQUERO instructions.
 * The aggregate function receives as inputs the tuples (an structure
 * of arrays) and the context (DBMS manager that holds the operations
 * to execute by the aggregation function.
 * The context has all the metadata regarding the operator, this inclu-
 * des the type of aggregation to be executed. This was initialized
 * by the DBMS similar to the next example:
 *
 * if (context->aggr_tuple_type == SUM)
 *    context->aggregator = vaquero_sum_aggr;
 *
 * Where vaquero_sum_aggr is a void* that execute the aggregation by
 * count using the associative mode.
 *
 * Additionally, the context includes the aggregation state. This structure
 * is implemented as a hash table with open addressing, thus, a hash func-
 * tion (also included in context) is executed to generate the addresses.
 *
 * NOTE: The algorithm is over-simplified to only highlight the VAQUERO
 *       processing section.
 */

inline void vaquero_sum_aggr(svbool_t mask, svuint32_t keys_v,
      svuint32_t values_v, Context* c) {
   // Execute the specified operation in VAQUERO
   // Associative mode addition (32-bit elements)
   //                   (mask, keys, values)
   svVaqAdd_asso_aspm_u32(mask, keys_v, values_v);

   // Check if there where evictions in the replacement buffer
   if (aspm_buffer_size() > 0) {
      // Read the evicted indices and values
      // aspm_read_buffer : 0->index buffer, 1->values buffer
      svuint32_t repl_key_v = aspm_read_buffer(0);
      svuint32_t repl_val_v = aspm_read_buffer(1);
      // Update the hash table
      c->update_table(repl_key_v, repl_val_v);
   }
}

void aggregate(HeapTuple * input, Context *context) {

   // First clear the ASPM
   clear_aspm();

   // Next, iterate on the tuples
   for (uint32_t it=0; it<input->num_tuples; it+=vlen) {
      // Execution mask
      it_m = svwhilelt_b32_u32(it, input->num_typles);
      // Load the keys and values from the input tuples into vector registers
      keys_v = svld1_u32(it_m, (input->keys + it));
      values_v = svld1_u32(it_m, (input->values + it));
      // Generate the hashed keys
      hashK_v = HASH(keys_v);
      // Execute the proper aggregation to the hashed keys and values
      context->aggregator(hashK_v, values_v, context);
   }

   // After traversing the input tuples we store the result back to the
   // aggregation state
   for (uint32_t it=0; it<ASPM.key_table.size; it+=vlen) {
      // Execution mask
      it_m = svwhilelt_b32_u32(it, ASPM.key_table.size);
      // Read the keys stored in VAQUERO
      keys_v = aspm_read_table(it);
      // Read the aggregated values in VAQUERO
      values_v = values_v = aspm_load_asso_u32(it_m, leys_v);
      // Update the aggregation state
      context->update_table(keys_v, values_v);
   }
}
