/*
 * Decorator
 *
 * 目的
 * オブジェクトに責任を動的に追加する。
 * Decoratorパターンは、サブクラス化よりも柔軟な機能拡張方法を提供する。
 *
 * 適用可能性
 * ・個々のオブジェクトに責任を動的、
 * 　かつ透明に（すなわち、他のオブジェクトには影響を与えないように）追加する場合。
 * ・責任を取りはずすことができるようにする場合。
 * ・サブクラス化による拡張が非実用的な場合。
 * 　非常に多くの独立した拡張が起こり得ることがある。
 * 　このような場合、サブクラス化によりすべての組み合わせの拡張に対応しようとすると、
 * 　莫大な数のサブクラスが必要になるだろう。
 * 　また、クラス定義が隠蔽されている場合や入手できない場合にも、このパターンを利用できる。
 */

// 責任を動的に追加できるようになっているオブジェクトのためのインタフェースを定義する。
class Component
{
	public:
		virtual void Operation(void);
};

// 責任を追加できるようになっているオブジェクト（component）を定義する。
class ConcreteComponent : public Component
{
};

// componentまたはdecoratorへの参照を保持し、
// またComponentクラスのインタフェースと一致したインタフェースを定義する。
class Decorator : public Component
{
	public:
		Decorator(Component* component)
			: component_(component) {}

		virtual void Operation(void)
		{
			component_->Operation();
		}

	protected:
		Component* component_;
};

// componentに責任を追加するオブジェクト（decorator）を定義する。
class ConcreteDecorator : public Decorator
{
		virtual void Operation(void)
		{
			// make_concrete_decoration();
			component_->Operation();
		}
};

