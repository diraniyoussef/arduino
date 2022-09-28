char* concat(const char *s1, const char *s2)
{
    char *result = (char *) malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcat(strcpy(result, s1), s2);
    return result;
}