/*
 * ChainOfResponsibility
 *
 * 目的
 * 1つ以上のオブジェクトに要求を処理する機会を与えることにより、
 * 要求を送信するオブジェクトと受信するオブジェクトの結合を避ける。
 * 受信する複数のオブジェクトをチェーン状につなぎ、
 * あるオブジェクトがその要求を処理するまで、そのチェーンに沿って要求を渡していく。
 *
 * 適用可能性
 * ・要求を処理するオブジェクトの候補が複数存在し、最終的にどのオブジェクトが担当するのかは、
 * 　前もってわからない場合。担当オブジェクトは自動的に決められる。
 * ・受け手を明確にせずに、複数あるオブジェクトの1つに対して要求を発行したい場合。
 * ・要求を処理することができるオブジェクトの集合が動的に明確化される場合。
 */

// 要求を処理するためのインタフェースを定義する。
// （オプション）successorへのリンクを実装する。
class Handler
{
	public:
		Handler(Handler* successor)
			: successor_(successor) {}

		virtual void Handle(void)
		{
			if(successor_ != 0)
			{
				successor_->Handle();
			}
		}

	protected:
		Handler* successor_;
};

// 要求の中で担当するものについて処理する。
// successorにアクセスできる。
// 要求を処理できるならば処理する。さもなければ、その要求をsuccessorに転送する。
class ConcreteHandler : public Handler
{
};

// チェーンの中にあるConcreteHandlerオブジェクトに要求を出す。
class Client
{
};

