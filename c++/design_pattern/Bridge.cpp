/*
 * Bridge
 *
 * 目的
 * 抽出されたクラスと実装を分離して、それらを独立に変更できるようにする。
 *
 * 適用可能性
 * ・抽出されたクラスとその実装を永続的に結合することを避けたい場合。
 * 　たとえば、実装を実行時に選択したり交換したりしなければならないときに、
 * 　このような場合が起こり得る。
 * ・抽出されたクラスとその実装の両方を、サブクラスの追加により拡張可能にすべき場合。
 * 　この場合、Bridgeパターンを用いることで、抽出されたクラスに異なる実装を結合したり、
 * 　それぞれを独立に拡張することが可能になる。
 * ・抽出されたクラスの実装における変更が、クライアントに影響を与えるべきではない場合。
 * 　すなわち、クライアントのコードを再コンパイルしなくても済むようにすべき場合。
 */

// 実装を行うクラスのインタフェースを定義する。
// このインタフェースはAbstractionクラスのインタフェースと正確に一致する必要はない。
// 実際、この2つのインタフェースがまったく異なることもあり得る。
// 典型的には、Implementorクラスのインタフェースはプリミティブなオペレーションのみを提供しており、
// Abstractionクラスは、これらのオペレーションを基にしてより高レベルのオペレーションを定義する。
class Implementor
{
	public:
		~Implementor() {};
		virtual void OperationImp(void) = 0;
};

// Implementorクラスのインタフェースを実装する。具体的な実装について定義する。
class ConcreteImplementor : public Implementor
{
	public:
		virtual void OperationImp(void) { ; }
};

// 抽出されたクラスのインタフェースを定義する。
// Implementor型のオブジェクトへの参照を保持する。
class Abstraction
{
	public:
		void Operation(void)
		{
			implementor_->OperationImp();
		}

	protected:
		Implementor* implementor_;
};

// Abstractionクラスで定義されたインタフェースを拡張する。
class RefinedAbstraction : public Abstraction
{
};

