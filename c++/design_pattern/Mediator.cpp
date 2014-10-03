/*
 * Mediator
 *
 * 目的
 * オブジェクト群の相互作用をカプセル化するオブジェクトを定義する。
 * Mediatorパターンは、オブジェクト同士がお互いを明示的に参照し合うことがないようにして、
 * 結合度を低めることを促進する。
 * それにより、オブジェクトの相互作用を独立に変えることができるようになる。
 *
 * 適用可能性
 * ・しっかりと定義されているが複雑な方法で、オブジェクトの集まりが通信する場合。
 * 　その結果、オブジェクト間の依存関係が構造化できず、理解が難しい。
 * ・あるオブジェクトが他の多くのオブジェクトに対して参照を持ち、それらと通信するので、
 * 　それを再利用するのが難しい場合。
 * ・いくつかのクラス間に分配された振る舞いを、できるだけサブクラス化を行わずにカスタマイズしたい場合。
 */

class Mediator;
class Colleague;

// Colleagueオブジェクトと通信するためのインタフェースを定義する。
class Mediator
{
};

// Colleagueオブジェクト間の調整を図ることにより、協調的な振る舞いを実装する。
// Colleagueオブジェクトを保持している。
class ConcreteMediator : public Mediator
{
	protected:
		Colleague* colleague_;
};

// 各ColleagueクラスがMediatorオブジェクトを知っている。
// 各Colleagueオブジェクトは、他のColleagueオブジェクトと通信しなければならないときには、
// 常にMediatorオブジェクトを介して行う。
class Colleague
{
	protected:
		Mediator* mediator_;
};
