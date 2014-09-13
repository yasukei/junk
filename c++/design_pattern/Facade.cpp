/*
 * Facade
 *
 * 目的
 * サブシステム内に存在する複数のインタフェースに1つの統一インタフェースを与える。
 * Facadeパターンはサブシステムの利用を容易にするための高レベルインタフェースを定義する。
 *
 * 適用可能性
 * ・複雑なサブシステムに単純なインタフェースを提供したい場合。
 * 　サブシステムは発展するにつれて、より複雑になっていく。
 * 　たいていのパターンは、適用するとたくさんの小さなクラスが導入されることになる。
 * 　それにより、サブシステムの再利用性が増し、カスタマイズも容易になる。
 * 　しかしその一方で、
 * 　サブシステムをカスタマイズする必要のないクライアントにとっては、そのサブシステムの利用が難しくなる。
 * 　このような場合に、facadeはサブシステムの単純なデフォルトのビューを提供してくれる。
 * 　ほとんどのクライアントにとってはこのデフォルトのビューだけで十分である。
 * 　サブシステムをカスタマイズする必要のあるクライアントだけが、
 * 　facadeを越えてサブシステムの内部まで見ることになる。
 * ・ある抽象を実装しているクラスとクライアントの間に多くの依存関係がある場合。
 * 　サブシステムをクライアントやほかのサブシステムから切り離して、独立性や移植性を高めるためにfacadeを導入する。
 * ・サブシステムを階層化したい場合。
 * 　各階層の各サブシステムへの入り口を定義するためにfacadeを使う。
 * 　複数のサブシステムが依存し合っている場合、
 * 　それらのサブシステムが互いにfacadeを通してのみやりとりを行うようにすれば、
 * 　それらの依存関係を単純にすることができる。
 */

// サブシステム内のどのクラスがクライアントからの要求に対して責任を負っているのかを知っている。
// クライアントからの要求をサブシステム内の適当なオブジェクトに委譲する。
class Facade
{
};

// サブシステムの機能を実装している。
// Facadeオブジェクトにより割り当てられた仕事を処理する。
// Facadeオブジェクトについては何も知らない。
// つまり、サブシステム内のクラスはFacadeオブジェクトへの参照を保持しない。
class ClassInSubsystem
{
};
