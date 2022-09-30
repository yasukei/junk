/*
 * Memento
 *
 * 目的
 * カプセル化を破壊せずに、オブジェクトの内部状態を捉えて外面化しておき、
 * オブジェクトを後にこの状態に戻すことができるようにする。
 *
 * 適用可能性
 * ・オブジェクトの状態（の一部）のスナップショットを、後にオブジェクトをその状態に戻すことができるように、
 * 　セーブしておかなければならない場合。
 * ・状態を得るための直接的なインタフェースが、実装の詳細を公開し、オブジェクトのカプセル化を破壊する場合。
 */

class Memento;
class Originator;
class Caretaker;
typedef int State;

// Originatorオブジェクトの内部状態を保存する。
// Mementoオブジェクトは、Originatorオブジェクトの内部状態のうち必要な部分だけをOriginatorオブジェクトの判断により保存する。
// Originator以外のオブジェクトによるアクセスから保護する。
// Mementoクラスには2つのインタフェースが効果的に備わっている。
// Caretakerクラスには、Mementoクラスのnarrowインタフェースが見えるようになっている
// （それはMementoオブジェクトを他のオブジェクトに渡すことができるようになるだけである）。
// それとは対照的に、Originatorクラスにはwideインタフェースが見えるようになっている。
// すなわち、そのインタフェースにより、
// Originatorオブジェクトを前の状態に戻すために必要なすべてのデータにアクセスできるようになっている。
// 理想的には、Mementoオブジェクトを生成したOriginatorオブジェクトだけが
// Mementoオブジェクトの内部構造にアクセスすることを許されるようにする。
class Memento
{
	protected:
		State GetState(void);
		State SetState(void);
		State state_;
};

// 内部状態のスナップショットを入れておくためにMementoオブジェクトを生成する。
// 内部状態を元に戻すためにMementoオブジェクトを使う。
class Originator
{
	public:
		void SetMemento(Memento* memento);
		Memento* CreateMemento(void);
	protected:
		State state_;
};

// Mementoオブジェクトを保管する責任がある。
// Mementoオブジェクトの内容を操作したり調べたりすることはない。
class Caretaker
{
	protected:
		Memento* memento;
};

