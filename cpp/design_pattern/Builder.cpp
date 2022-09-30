/*
 * Builder
 *
 * 目的
 * 複合オブジェクトについて、その作成過程を表現形式に依存しないものにすることにより、
 * 同じ作成過程で異なる表現形式のオブジェクトを生成できるようにする。
 *
 * 適用可能
 * ・多くの構成要素からなるオブジェクトを生成するアルゴリズムを、
 * 　構成要素自体やそれらがどのように組み合わされるのかということから独立にしておきたい場合。
 * ・オブジェクトの作成プロセスが、
 * 　オブジェクトに対する多様な表現を認めるようにしておかなければならない場合。
 */

// 作成中の、多くの構成要素からなる複合オブジェクトを表す。
// ConcreteBuilderクラスは、Productオブジェクトの内部表現を作成し、また、それを組み立てる過程を定義している。
// 構成要素を定義するクラス、および構成要素を最終的なProductオブジェクトに組み合わせていくためのインタフェースを含んでいる。
class Product
{
};

// Productオブジェクトの構成要素を生成するための抽象化されたインタフェースを提供する
class Builder
{
	public:
		~Builder() {};
		virtual void BuildPartA(void) {}; // 純仮想関数にしない。サブクラスが必要に応じてオーバーライドする。
		virtual void BuildPartB(void) {};
		virtual void BuildPartC(void) {};
};

// Builderクラスのインタフェースを実装することで、Productオブジェクトの構成要素の生成や組み合わせを行う。
// 自身が生成する表現を定義し、管理する。
// Productオブジェクトを取り出すためのインタフェースを提供する。
class ConcreteBuilder : public Builder
{
	public:
		void BuildPartA(void);
		Product* GetResult(void);
};

// Builderクラスのインタフェースを使って、オブジェクトを生成する。
class Director
{
	public:
		Director(Builder* builder)
			: builder_(builder)
		{}

		void Construct(void)
		{
			builder_->BuildPartA();
			builder_->BuildPartB();
			builder_->BuildPartC();
		}

	protected:
		Builder* builder_;
};

class Client
{
	void Method(void)
	{
		ConcreteBuilder* builder = new ConcreteBuilder();
		Director* director = new Director(builder);

		director->Construct();
		builder->GetResult();
	}
};
