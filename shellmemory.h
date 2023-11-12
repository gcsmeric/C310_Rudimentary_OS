void mem_init();
void mem_clear_interval(int start, int end);
char *mem_get_value(char *var);
void printMemory ();
int mem_set_value(char *var, char *value);
int frame_store_set_value(char *var_in, char *value_in);
char *frame_store_get_value(char *var_in);
int empty_space_check ();
void removeNone2 ();
void fill_in_frame_store ();
int getFirstEmptyLine ();
void frame_store_set_value_by_index(char *var_in, char *value_in, int start_int);
char *frame_store_get_value_at_index (int i);
void frame_store_set_value_at_index(int index, char* var, char* value);
int get_frame_store_length ();
// function that resets variable store
void print_sizes ();
void resetmem ();
void resetframestore ();
void print_frame_store ();
void print_variable_store ();