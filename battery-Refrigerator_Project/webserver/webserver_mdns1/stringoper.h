class Buff {
  private:

	const static unsigned int MAX_LEN = 40;
	unsigned int len = 0;		
	
  public:
	
	char buff[MAX_LEN];

	Buff() {
		reset();
	}

	bool add(char c) {
		if( len < MAX_LEN ) {
			buff[len] = c;
			len += 1;
			buff[len] = '\0';
			return true;
		}
		return false;
	}

	void reset() {
		buff[0] = '\0';
		len = 0;
	}

	bool has(char* str) {
		if(str == NULL || str == "") {
			return false;
		}
		unsigned int str_len = 0;
		for(str_len=0; str[str_len] != '\0'; str_len++) {}

		for(int i=0; i<len; i++) {
			bool exists = true;
			for(int j=0; j<str_len; j++){
				if( i + j >= len || buff[i+j] != str[j]) {
					exists = false;
					break;
				}
			}
			if(exists){
				return true;
			}
		}
		return false;
	}
};