namespace Qwr.ComponentInterface
{
    public interface IConfigVar<T>
    {
        void Set(T value);
        T Get();
    }
}