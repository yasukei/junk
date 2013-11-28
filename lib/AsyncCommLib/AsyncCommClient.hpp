#ifndef ASYNCCOMMCLIENT_HPP_
#define ASYNCCOMMCLIENT_HPP_

//class ACCRequest
//{
//};
//
//class ACCService
//{
//	public:
//		// constructor
//		ACCService();
//
//		// copyConstructor
//		ACCService(const ACCService& rhs);
//
//		// destructor
//		~ACCService();
//
//		// substitution operator
//		AsyncCommClient& operator=(const ACCService& rhs);
//
//	private:
//};
//
//class ACCSendService : public ACCService
//{
//	public:
//		// constructor
//		ACCSendService();
//
//		// copyConstructor
//		ACCSendService(const ACCSendService& rhs);
//
//		// destructor
//		~ACCSendService();
//
//		// substitution operator
//		ACCSendService& operator=(const ACCSendService& rhs);
//
//	private:
//};

class AsyncCommClient
{
	public:
		// constructor
		AsyncCommClient();

		// copyConstructor
		//AsyncCommClient(const AsyncCommClient& rhs);

		// destructor
		~AsyncCommClient();

		// substitution operator
		//AsyncCommClient& operator=(const AsyncCommClient& rhs);

	//private:
		//ACCSendService sendService;
		//ACCRequest requests[8];
};

#endif /* ASYNCCOMMCLIENT_HPP_ */
