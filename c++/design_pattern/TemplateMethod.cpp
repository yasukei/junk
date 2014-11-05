/*
 * TemplateMethod
 *
 * 目的
 * 1つのオペレーションにアルゴリズムのスケルトンを定義しておき、その中のいくつかのステップについては、
 * サブクラスでの定義に任せることにする。
 * Template Methodパターンでは、アルゴリズムの構造を変えずに、
 * アルゴリズム中のあるステップをサブクラスで定義する。
 *
 * 適用可能性
 * ・アルゴリズムの不変な部分をまず実装し、振る舞いが変わり得る部分の実装はサブクラスに残しておく場合。
 * ・同じコードがいたるところに現れることがないように、
 * 　サブクラス間で共通の振る舞いをする部分は抜き出して、これを共通のクラスに局所化する場合。
 * ・サブクラスの拡張を制御する場合。
 * 　特定の辞典で"hook" operationを呼び出すテンプレートメソッドを定義することができる。
 * 　それにより、このポイントでのみ拡張が許されることになる。
 */

// 抽象化されたprimitive operationを定義する。
// このオペレーションは具象サブクラスで定義され、アルゴリズムの各ステップを実装することになる。
// アルゴリズムのスケルトンを定義するtemplate methodを実装する。
// template methodは、AbstractClassクラスで定義されるオペレーションやその他のオブジェクトのオペレーションと同様に、
// primitive operationを呼び出す。
class AbstractClass
{
	public:
		void TemplateMethod(void)
		{
			primitiveOperation1();
			primitiveOperation2();
		}

	protected:
		void primitiveOperation1(void);
		void primitiveOperation2(void);
};

// アルゴリズムのステップを特有の形で実行するために、primitive operationを定義する。
class ConcreteClass : public AbstractClass
{
};
