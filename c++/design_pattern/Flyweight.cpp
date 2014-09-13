/*
 * Flyweight
 *
 * 目的
 * 多数の細かいオブジェクトを効率よくサポートするために共有を利用する。
 *
 * 適用可能性
 * ・アプリケーションが非常に多くのオブジェクトを利用する。
 * ・大量オブジェクトのために、メモリ消費コストが高くつく。
 * ・オブジェクトの状態を構成するほとんどの情報をextrinsicにできる。
 * ・extrinsic状態が取り除かれれば、
 * 　オブジェクトのグループの多くを比較的少数の共有オブジェクトに置き換えることができる。
 * ・アプリケーションがオブジェクトの同一性に依存しない。
 * 　flyweightオブジェクトは共有されている可能性があるため、
 * 　概念的には異なるオブジェクトなのだが、同一性テストの結果は真になってしまうことがあるだろう。
 */

// flyweightがextrinsic状態を受け取り、それに基づいて行動できるようにするためのインタフェースを定義する。
class Flyweight
{
	public:
		Flyweight(int key);
		virtual void Operation(void);
};

// Flyweightクラスのインタフェースを実装し、intrinsic状態があればその格納場所を追加する。
// ConcreteFlyweightオブジェクトは共有可能でなければならない。
// ConcreteFlyweightオブジェクトが格納する状態はすべてintrinsicでなければならない。
// すなわち、それは文脈に依存してはならない。
class ConcreteFlyweight
{
};

// Flyweightのサブクラスのすべてが共有可能になっている必要はない。
// Flyweightクラスのインタフェースは共有を可能にしてはいるが、共有を強制するわけではない。
// UnsharedConcreteFlyweightオブジェクトは、flyweightからなるオブジェクト構造において階層を形成し、
// ConcreteFlyweightオブジェクトを子として管理する役目を持つのが一般的である。
class UnsharedConcreteFlyweight
{
};

// flyweightを生成し、管理する。
// flyewightが正しく共有されることを保証する。
// Clientオブジェクトがflyweightを要求したとき、
// FlyweightFactoryオブジェクトはそのインスタンスが存在する場合にはそれを与え、
// 存在しない場合には新たに生成する。
class FlyweightFactory
{
	public:
		Flyweight* GetFlyweight(int key)
		{
			if(flyweights_[key] != (void*)0) {
				return flyweights_[key];
			} else {
				return flyweights_[key] = new Flyweight(key);
			}
		}

	protected:
		Flyweight* flyweights_[32];
};

// 1つ、あるいは複数のflyweightへの参照を保持する。
// flyweightのextrinsic状態を計算するか、または格納する。
class Client
{
};

