
#ifndef H_MAIN
#define H_MAIN

// class CObjectFactory{
// public:
// 	CObjectFactory(){

// 	}

// 	void createObject(std::string ObjName){

// 	}
// };


int main(){
	TEST_CLASS_NAME app;

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif