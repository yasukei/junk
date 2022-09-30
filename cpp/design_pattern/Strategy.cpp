/*
 * Strategy
 *
 * 目的
 * アルゴリズムの集合を定義し、各アルゴリズムをカプセル化して、それらを交換可能にする。
 * Strategyパターンを利用することで、アルゴリズムを、
 * それらを利用するクライアントからは独立に変更することができる。
 *
 * 適用可能性
 * ・関連する多くのクラスが振る舞いのみ異なっている場合。
 * 　Strategyパターンは、多くの振る舞いの中の1つでクラスを構成する方法を提供する。
 * ・複数の異なるアルゴリズムを必要とする場合。
 * 　たとえば、空間と時間のトレードオフを反映する複数のアルゴリズムを定義する場合が考えられる。
 * 　このとき、複数のアルゴリズムをクラス階層として実装していく際に、Strategyパターンを利用することができる。
 * ・アルゴリズムが、クライアントが知るべきではないデータを利用している場合。
 * 　Strategyパターンを利用することにより、複雑でアルゴリズムに特有なデータ構造を公開するのを避けることができる。
 * ・クラスが多くの振る舞いを定義しており、これらがオペレーション内で複数の条件文として現れている場合。
 * 　このとき、多くの条件文を利用する代わりに、条件分岐後の処理をStrategyクラスに移し換える。
 */

// サポートするすべてのアルゴリズムに共通のインタフェースを宣言する。
// Contextクラスは、ConcreteStrategyクラスにより定義されるアルゴリズムを呼び出すためにこのインタフェースを利用する。
class Strategy
{
	public:
		void AlgorithmInterface(void);
};

// Strategyクラスのインタフェースを利用して、アルゴリズムを実装する。
class ConcreteStrategyA : public Strategy
{
};

class ConcreteStrategyB : public Strategy
{
};

// ConcreteStrategyオブジェクトを備えている。
// Strategyのオブジェクトに対する参照を保持する。
// StrategyクラスがContextクラスのデータにアクセスするためのインタフェースを定義してもよい。
class Context
{
	public:
		void ContextInterface(void);

	protected:
		Strategy* strategy_;
};

