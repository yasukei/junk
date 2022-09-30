/*
 * Observer
 *
 * 目的
 * あるオブジェクトが状態を変えたときに、それに依存するすべてのオブジェクトに自動的にそのことが知らされ、
 * また、それらが更新されるように、オブジェクト間に一対多の依存関係を定義する。
 *
 * 適用可能性
 * ・抽象化により、2つの面が、一方が他方に依存しているという形で現れる場合。
 * 　これらの面をそれぞれ別々のオブジェクトにカプセル化することにより、それらを独立に変更したり、
 * 　再利用することが可能になる。
 * ・1つのオブジェクトを変化させるときに、それに伴いその他のオブジェクトも変化させる必要があり、
 * 　しかも変化させる必要があるオブジェクトを固定的に決められない場合。
 * ・オブジェクトが、他のオブジェクトに対して、それがどのようなものなのか仮定せずに通知できるようにする場合。
 * 　別の言い方をすると、これらのオブジェクトを密に結合したくない場合。
 */

#include <vector>

class Subject;
class Observer;
class ConcreteSubject;
class ConcreteObserver;

// observerを知っている。任意の数のobserverがsubjectの変化に対応している。
// observerを加えたりはずしたりするためのインタフェースを提供する。
class Subject
{
	public:
		void Attach(Observer* observer);
		void Detach(Observer* observer);
		void Notify(void);

	protected:
		std::vector<Observer> observers_;
};

// subject内の変化が通知されたときのために、更新のインタフェースを定義する。
class Observer
{
	public:
		void update(void);
};

// ConcreteObserverオブジェクトに影響する状態を保存している。
// 状態が変わったときにConcreteObserverオブジェクトに通知を送る。
class ConcreteSubject : public Subject
{
};

// ConcreteSubjectオブジェクトへの参照を保持している。
// その状態をConcreteSubjectオブジェクトの状態と矛盾しないようにして保存している。
// その状態をConcreteSubjectオブジェクトの状態と矛盾しないようにしておくために、
// Observerクラスで宣言した更新のインタフェースを実装する。
class ConcreteObserver : public Observer
{
};


