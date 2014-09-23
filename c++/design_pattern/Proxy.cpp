/*
 * Proxy
 *
 * 目的
 * あるオブジェクトへのアクセスを制御するために、そのオブジェクトの代理、または入れ物を提供する。
 *
 * 適用可能性
 * ・参照されなくなった実オブジェクトを自動的に解放できるように、実オブジェクトへの参照個数を数えておく。
 * ・永続オブジェクトが初めて参照されたときに、それをメモリ上にロードする。
 * ・他のオブジェクトが実オブジェクトを変更することのないように、実オブジェクトがアクセスされる前に
 * 　それがロックされていることを確認する。
 */

// RealSubjectオブジェクトにアクセスするための参照を保持する。
// ReadSubjectクラスのインタフェースとSubjectクラスのインタフェースが等しい場合には、
// ProxyクラスはSubjectのオブジェクトを参照するようにしておくこともできる。
// ProxyオブジェクトをRealSubjectオブジェクトと置き換えられるように、
// Subjectクラスと同一のインタフェースを提供する。
// RealSubjectオブジェクトへのアクセスを提供する。また、RealSubjectオブジェクトの生成や消去に責任を持つこともある。
// 他の責任はproxyの種類に依存する。
class Subject
{
};

// Proxyオブジェクトがその代理を務めることになる実オブジェクトを定義する。
class RealSubject : public Subject
{
};

// RealSubjectオブジェクトを利用できるところならばどこでもProxyオブジェクトを利用できるように、
// RealSubjectクラスとProxyクラスに共通のインタフェースを定義する。
class Proxy : public Subject
{
	protected:
		RealSubject* realSubject_;
		//Subject* realSubject_; // altanative
};

