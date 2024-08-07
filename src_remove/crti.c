
void* __stack_top;

void application_init(void);
void application_clean(void);

void __custom_init(void){
    application_init();
}

void __custom_fini(void){
    application_clean();
}
