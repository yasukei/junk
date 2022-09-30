/*
 * Adapter
 *
 * 目的
 * あるクラスのインタフェースを、クライアントが求める他のインタフェースへ変換する。
 * Adapterパターンは、インタフェースに互換性のないクラス同士を組み合わせることができるようにする。
 *
 * 適用可能性
 * ・既存のクラスを利用したいが、そのインタフェースが必要なインタフェースと一致していない場合。
 * ・まったく無関係で予想もつかないようなクラス（必ずしも互換性のあるインタフェースを持つとは限らない）
 * 　とも協調していける、再利用可能なクラスを作成したい場合。
 * ・（オブジェクトに適用するAdapterパターンのみ）既存のサブクラスを複数利用したいが、
 * 　それらすべてのサブクラスをさらにサブクラス化することで、そのインタフェースを適合させることが現実的でない場合。
 * 　オブジェクトに適用するAdapterパターンでは、その親クラスのインタフェースを適合させればよい。
 */

// 適合させる必要のある既存のインタフェースを持つ。
class Adaptee
{
	public:
		void SpecialRequest(void);
};

// Clientクラスが利用する、ドメインに特化したインタフェースを定義する。
class Target
{
	public:
		void Request(void);
};

// AdapteeクラスのインタフェースをTargetクラスのインタフェースに適合させる。
class Adapter : public Target
{
	public:
		void Request(void)
		{
			adaptee_->SpecialRequest();
		}

	protected:
		Adaptee* adaptee_;
};

// Targetクラスのインタフェースに従ったオブジェクトと協力する。
class Client
{
	void Operation(Target* target)
	{
		target->Request();
	}
};
