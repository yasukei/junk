/*
 * Visitor
 *
 * 目的
 * あるオブジェクト構造上の要素で実行されるオペレーションを表現する。
 * Visitorパターンにより、オペレーションを加えるオブジェクトのクラスに変更を加えずに、
 * 新しいオペレーションを定義することができるようになる。
 *
 * 適用可能性
 * ・オブジェクト構造にインタフェースが異なる多くのクラスのオブジェクトが存在し、
 * 　これらのオブジェクトに対して、各クラスで別々に定義されているオペレーションを実行したい場合。
 * ・関連のない異なるオペレーションをオブジェクト構造の中のオブジェクトに対して実行する必要があり、
 * 　さらに、これらのオペレーションをクラスに持たせることで、クラスを"汚くする"ことを避けたい場合。
 * 　visitorを利用すれば関連するオペレーションを1つのクラスの中に定義するので、
 * 　それらをまとめておくことができるようになる。
 * 　オブジェクト構造が多くのアプリケーションで共有されるときには、
 * 　オペレーションをそれらのアプリケーションで共通に使うことができるようにVisitorパターンを利用する。
 * ・オブジェクト構造を定義するクラスはほとんど変わらないが、
 * 　その構造に新しいオペレーションを定義することがしばしば起こる場合。
 * 　オブジェクト構造のクラスを変更する場合には、すべてのvisitorのインタフェースを再定義する必要があり、
 * 　潜在的にコストは高くつく。
 * 　もしオブジェクト構造のクラスを変更することがしばしばあるならば、
 * 　それらのクラスにオペレーションを定義しておく方がよいだろう。
 */

// 
class Visitor
{
};
