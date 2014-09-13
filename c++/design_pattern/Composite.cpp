/*
 * Composite
 *
 * 目的
 * 部分-全体階層を表現するために、オブジェクトを木構造に組み立てる。
 * Compositeパターンにより、
 * クライアントは、個々のオブジェクトとオブジェクトを合成したものを一様に扱うことができるようになる。
 *
 * 適用可能性
 * ・オブジェクトの部分-全体階層を表現したい場合。
 * ・クライアントが、オブジェクトを合成したものと個々のオブジェクトの違いを無視できるようにしたい場合。
 * 　このパターンを用いることで、
 * 　クライアントは、composite構造内のすべてのオブジェクトを一様に扱うことができるようになる。
 */

// composite内のオブジェクト（componentと呼ぶ）のインタフェースを宣言する。
// すべてのクラスに共通なインタフェースのデフォルトな振る舞いを適宜実装する。
// 子にあたるComponentオブジェクトにアクセスしたり、それを管理するためのインタフェースを宣言する。
// （オプション）再起構造において、
// 親にあたるcompositeにアクセスするためのインタフェースを宣言しておき、適宜実装する。
class Component
{
	public:
		virtual void Operation(void);
		virtual void Add(Component* component);
		virtual void Remove(Component* component);
		virtual Component* GetChild(int index);
};

// composite内の末端のオブジェクトを表す。
// つまり、leafは子オブジェクトを持たない。
// composite内のプリミティブなオブジェクトの振る舞いを定義する。
class Leaf : public Component
{
	public:
		virtual void Operation(void) { ; }
		virtual void Add(Component* component) { ; }
		virtual void Remove(Component* component) { ; }
		virtual Component* GetChild(int index) { ; }
};

// 子オブジェクトを持つcomponentの振る舞いを定義する。
// 子にあたるcomponentを保持する。
// Componentクラスのインタフェースで宣言された、子オブジェクトに関するオペレーションを実装する。
class Composite : public Component
{
	public:
		virtual void Operation(void);
		virtual void Add(Component* component);
		virtual void Remove(Component* component);
		virtual Component* GetChild(int index);

	protected:
		Component* children_;
		int numofChildren_;
};

// Componentクラスのインタフェースを通して、composite内のオブジェクトを操作する。
class Client
{
};

