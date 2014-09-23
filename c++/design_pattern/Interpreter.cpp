/*
 * Interpreter
 *
 * 目的
 * 言語に対して、文法表現と、それを使用して文を解釈するインタプリタを一緒に定義する。
 *
 * 適用可能性
 * ・文法が単純な場合。
 * 　文法が複雑な場合には文法を表現するクラス階層が大きくなり、管理できなくなる。
 * 　そのような場合には、パーザジェネレータのようなツールの方が適している。
 * 　パーザジェネレータでは、アブストラクト・シンタックスツリーを構築せずに表現を解釈するので、
 * 　メモリと処理時間を節約することができる。
 * ・効率が重要な関心事ではない場合。
 * 　もっとも効率的なインタプリタは、通常は、構文解析木を直接解釈するのではなく、
 * 　最初に別の形に変換するように実装されている。
 * 　たとえば、正規表現はしばしば状態機械に変換される。しかし、そのような場合でも、
 * 　Interpreterパターンを適用して変換を実装することができる。
 */

/* ※以下の実装はInterpreterパターンをよく示しているわけではないので、あまり参考にすべきではない。 */

// インタプリタにとって、グローバルな情報を含んでいる。
class Context
{
	public:
		void Assign(void* context);
};

// アブストラクト・シンタックスツリーのすべてのノードに共通な抽象化されたInterpretオペレーションを宣言する。
class AbstractExpression
{
	public:
		virtual void Interpret(Context* context);
};

// 文法中の終端器号に関するInterpretオペレーションを実装する。
// 文中の1つ1つの終端記号について、インスタンスが生成される。
class TerminalExpression : public AbstractExpression
{
};

// 文法中のR::=R1R2...Rnの形で表された規則1つ1つについて、このクラスが定義される。
// R1からRnの各記号について、AbstractExpressionの型のインスタンス変数を保持する。
// 文法中の非終端記号についてInterpretオペレーションを実装する。Interpretオペレーションは、
// 典型的には、R1からRnで表された変数上で、自身を再帰的に呼び出していく。
class NonterminalExpression : public AbstractExpression
{
	public:
		NonterminalExpression(AbstractExpression* abstractExpression)
			: abstractExpression_(abstractExpression)
		{}

	protected:
		AbstractExpression* abstractExpression_;
};

// 文法により定められた言語において、文を表現するアブストラクト・シンタックスツリーを作る（または、与えられる）。
// アブストラクト・シンタックスツリーは、
// NonterminalExpressionクラスやTerminalExpressionクラスのインスタンスから組み立てられる。
// Interpretオペレーションの呼び出しを行う。
class Client
{
	void AnOperation(void)
	{
		Context* context = new Context();
		TerminalExpression* te = new TerminalExpression();
		NonterminalExpression* nte = new NonterminalExpression(te);

		context->Assign(0);
		nte->Interpret(context);
	}
};
