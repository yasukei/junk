/*
 * Factory Method
 *
 * 目的
 * オブジェクトを生成するときのインタフェースだけを規定して、
 * 実際にどのクラスをインスタンス化するかはサブクラスが決めるようにする。
 * Factory Methodパターンは、インスタンス化をサブクラスに任せる。
 *
 * 適用可能性
 * ・クラスが、生成しなければならないオブジェクトのクラスを事前に知ることができない場合。
 * ・サブクラス化により、生成するオブジェクトを特定化する場合
 * ・クラスが責任をいくつかのサブクラスの中の1つに委譲するときに、
 * 　どのサブクラスに委譲するのかに関する知識を局所化したい場合。
 */

// factory methodが生成するオブジェクトのインタフェースを定義する。
class Product
{
};

// Productクラスのインタフェースを実装する。
class ConcreteProduct : public Product
{
};

// Product型のオブジェクトを返すfactory methodを宣言する。
// また、あるProductオブジェクトを返すようにfactory methodの実装をデフォルトで定義することもある。
// Productオブジェクトを生成するためにfactory methodを呼び出す。
class Creator
{
	Product* FactoryMethod(void)
	{
		return new Product();
	}

	void AnOperation(void)
	{
		product_ = FactoryMethod();
	}

	Product* product_;
};

// ConcreteProductクラスのインスタンスを返すように、factory methodをオーバーライドする。
class ConcreteCreator : public Creator
{
	Product* FactoryMethod(void)
	{
		return new ConcreteProduct();
	}
};
