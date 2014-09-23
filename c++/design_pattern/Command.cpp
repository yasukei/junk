/*
 * Command
 *
 * 目的
 * 要求をオブジェクトとしてカプセル化することによって、
 * 異なる要求や、要求からなるキューやログにより、クライアントをパラメータ化する。
 * また、取り消し可能なオペレーションをサポートする。
 *
 * 適用可能性
 * ・先にあげたMenuItemオブジェクトのように、実行する動作によりオブジェクトをパラメータ化したい場合。
 * 　手続き型言語では、そのようなパラメータ化をコールバック関数を使って表現する。
 * 　すなわち、コールバック関数を、呼び出してほしいところに登録しておく、という形になる。
 * 　Commandパターンでは、そのようなコールバック関数の代わりにオブジェクトを使う。
 * ・要求を明確にし、順番に並べ、実行するのをそれぞれ別々に行いたい場合。
 * 　commandは、元の要求とは独立に存在し続ける。
 * 　もし、要求を最終的に受信するオブジェクトをアドレス空間とは独立な方法で表現できるならば、
 * 　commandを別のプロセスに移してそこで要求を実行させることもできる。
 * ・要求の取り消しをサポートしたい場合。
 * 　CommandクラスのExecuteオペレーションでは、commandでの処理の結果を再び元の状態に戻すことができるように、
 * 　状態を保存するようにしておくことができる。その場合Commandクラスには、インタフェースとして、
 * 　直前のExecuteオペレーションの呼び出しの結果を元に戻すUnexecuteオペレーションを追加しておかなければならない。
 * 　実行されるcommandは、履歴リストの中にたくわえられる。取り消しや再実行は、このリスト内を前後に移動しながら、
 * 　UnexecuteオペレーションやExecuteオペレーションの呼び出しを行うことにより、何度でも実行できる。
 * ・システムがクラッシュしたときにコマンドを再度実行できるように、ログの更新をサポートしたい場合。
 * 　Commandクラスのインタフェースにロードとセーブのオペレーションを備えておくことで、
 * 　更新を永続的にログに記録しておくことができる。クラッシュからの復旧に際しては、
 * 　ログに記録されたcommand群をディスクからロードして、それらに対してExecuteオペレーションの呼び出しを再度行う。
 * ・プリミティブなオペレーションを基に作られた高度なオペレーションによりシステムを構造化したい場合。
 * 　そのような構造は、情報システムのトランザクションにとっては一般的である。
 * 　一般に、1つのトランザクションは、データに対する更新手続きの集合をカプセル化している。
 * 　Commandパターンはこのようなトランザクションをモデル化する方法を与える。
 * 　commandは共通のインタフェースを持っているので、すべてのトランザクションを同じように呼び出すことができる。
 * 　このパターンにより、新しいトランザクションを追加してシステムを拡張することも容易になる。
 */

// 要求を実現するためにオペレーションをいかに実行するのかを知っている。
// 任意のクラスがReceiverになり得る。
class Receiver
{
	public:
		void Action(void);
};

// オペレーションを実行するためのインタフェースを宣言する。
class Command
{
	public:
		virtual void Execute(void);
};

// Receiverオブジェクトとアクションの間のつながりを定義する。
// Executeオペレーションを、Receiverオブジェクトに対して該当するオペレーションの呼び出しを行うように実装する。
class ConcreteCommand : public Command
{
	public:
		ConcreteCommand(Receiver* receiver)
			: receiver_(receiver)
		{}

		virtual void Execute(void)
		{
			receiver_->Action();
		}

	protected:
		Receiver* receiver_;
};

// commandに要求を実行するように依頼する。
class Invoker
{
	public:
		void StoreCommand(Command* command)
		{
			command_ = command;
		}
		void NotifyEvent(void)
		{
			command_->Execute();
		}

	protected:
		Command* command_;
};

// ConcreteCommandオブジェクトを生成して、それに対するReceiverオブジェクトを設定する。
class Client
{
	void Initialize(Invoker* invoker)
	{
		Receiver* receiver = new Receiver();
		ConcreteCommand* command = new ConcreteCommand(receiver);

		invoker->StoreCommand(command);
	}
};

