int real_function() {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%s", "shkip");
}
int real_function2() {
    char buf[32];
    std::snprintf(buf, sizeof(buf), 
              "%s",
              "shkip");
}
