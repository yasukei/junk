/*
 * Iterator
 *
 * 目的
 * 集約オブジェクトが基にある内部表現を公開せずに、その要素に順にアクセスする方法を提供する。
 *
 * 適用可能性
 * ・集約オブジェクトの内部表現を公開せずに、その中にあるオブジェクトにアクセスしたい場合。
 * ・集約オブジェクトに対して、複数の走査をサポートしたい場合。
 * ・異なる集約構造の走査に対して、単一のインタフェースを提供したい
 * 　（すなわち、ポリモルフィックなiterationをサポートしたい）場合。
 */

class Iterator;
class Aggregate;
class Item;

// 要素にアクセスしたり走査したりするためのインタフェースを定義する。
class Iterator
{
	public:
		virtual Item* First(void);
		virtual Item* Next(void);
		virtual bool isDone(Item* item);
		virtual Item* CurrentItem(void);
};

// Iteratorクラスで定義したインタフェースを実装する。
// Aggregateオブジェクトの走査の際に、カレント要素を記録する。
class ConcreteIterator : public Iterator
{
};

// Iteratorオブジェクトを生成するためのインタフェースを定義する。
class Aggregate
{
	public:
		virtual Iterator* CreateIterator(void);
};

// Aggregateクラスで定義したインタフェースに対して、
// 適切なConcreteIteratorクラスのインスタンスを生成して返すように実装する
// （ConcreteAggregateクラスのインスタンスを、総称してaggregateと呼ぶ）。
class ConcreteAggregate : public Aggregate
{
	public:
		virtual Iterator* CreateIterator(void)
		{
			return new ConcreteIterator();
		}
};
