/*
 * Prototype
 *
 * 目的
 * 生成すべきオブジェクトの種類を原型となるインスタンスを使って明確にし、
 * それをコピーすることで新たなオブジェクトの生成を行う。
 *
 * 適用可能性
 * ・インスタンス化されるクラスが、
 * 　たとえば、ダイナミックローディングにより、実行時に明らかになる場合。
 * ・生成されるオブジェクトのクラス階層とパラレルな関係になるfactoryのクラス階層を作ることを避けたい場合。
 * ・クラスのインスタンスが、状態の数少ない組み合わせの中の1つを取る場合。
 * 　この可能な組み合わせ1つ1つに相当するインスタンスをprototypeとしてあらかじめ用意しておき、
 * 　その複製を行う方が、毎回クラスを適当な状態でインスタンス化するよりも便利であろう。
 */

class Prototype
{
	public:
		~Prototype() {};
		virtual Prototype* Copy(void) = 0;
};

class ConcretePrototype : public Prototype
{
	public:
		virtual Prototype* Copy(void)
		{
			return new ConcretePrototype();
		}
};

class Client
{
	void Operation(Prototype* prototype)
	{
		Prototype* p = prototype->Copy();
	}
};

