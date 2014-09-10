/*
 * Abstract Factory
 *
 * 目的
 * 互いに関連したり依存し合うオブジェクト群を、
 * その具象クラスを明確にせず生成するためのインタフェースを提供する
 *
 * 適用可能性
 * ・システムを、部品の生成、組み合わせ、表現の方法から独立にすべき場合。
 * ・部品の集合が複数存在して、その中の1つを選んでシステムを構築する場合。
 * ・一群の関連する部品を常に使用しなければならないように設計する場合。
 * ・部品のクラスライブラリを提供する際に、インタフェースだけを公開して、
 * 　実装は非公開にしたい場合。
 */

// 部品ごとにインタフェースを宣言する。
class AbstractProduct_A
{
};
class AbstractProduct_B
{
};

// AbstractProductのオブジェクトを生成するオペレーションのインタフェースを宣言する。
class AbstractFactory
{
	public:
		~AbstractFactory() {};
		virtual AbstractProduct_A* CreateProduct_A(void) = 0;
		virtual AbstractProduct_B* CreateProduct_B(void) = 0;
};

// 対応するConcreteFactoryオブジェクトで生成される部品オブジェクトを定義する。
// AbstractProductクラスのインタフェースを実装する。
class ConcreteProduct_A1 : public AbstractProduct_A
{
};
class ConcreteProduct_B1 : public AbstractProduct_B
{
};
class ConcreteProduct_A2 : public AbstractProduct_A
{
};
class ConcreteProduct_B2 : public AbstractProduct_B
{
};

// ConcreteProductオブジェクトを生成するオペレーションを実装する。
class ConcreteFactory1 : public AbstractFactory
{
	public:
		virtual AbstractProduct_A* CreateProduct_A(void) { return new ConcreteProduct_A1(); }
		virtual AbstractProduct_B* CreateProduct_B(void) { return new ConcreteProduct_B1(); }
};
class ConcreteFactory2 : public AbstractFactory
{
	public:
		virtual AbstractProduct_A* CreateProduct_A(void) { return new ConcreteProduct_A2(); }
		virtual AbstractProduct_B* CreateProduct_B(void) { return new ConcreteProduct_B2(); }
};

// AbstractFactoryクラスとAbstractProductクラスで宣言されたインタフェースのみを利用する。
class Client
{
	Client(AbstractFactory& factory)
	{
		Product_A_ = factory.CreateProduct_A();
		Product_B_ = factory.CreateProduct_B();
	}

	AbstractProduct_A* Product_A_;
	AbstractProduct_B* Product_B_;
};
