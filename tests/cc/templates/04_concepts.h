
template <typename T>
concept Integral = std::is_integral_v<T>; // Требует целочисленного типа


template <typename T>
concept SignedIntegral = Integral<T> && std::is_signed_v<T>;


template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>; // Проверяет возможность сложения
};


// Использование концептов

// а) Ограничение шаблонов:

// Синтаксис 1: после template
template <Integral T>
T square(T x) { return x * x; }

// Синтаксис 2: предложение requires
template <typename T>
requires Addable<T>
T add(T a, T b) { return a + b; }

// Синтаксис 3: рядом с объявлением функции
template <typename T>
T half(T x) requires std::floating_point<T> { return x / 2; }

// Синтаксис 4: сокращённая запись (auto + концепт)
void print(SignedIntegral auto x) {
    std::cout << x;
}


// б) Перегрузка шаблонов:

void process(Integral auto x);   // Версия для целых чисел
void process(std::floating_point auto x); // Версия для чисел с плавающей точкой



template <typename It>
concept RandomAccessIterator = requires(It it, int n) {
    { it + n } -> std::same_as<It>; // Проверка операции
    { it[n] } -> std::convertible_to<typename It::value_type>;
};

template <RandomAccessIterator It>
void sort(It begin, It end) { ... }


// Использование requires


// После template
template <typename T>
requires std::integral<T>
T add(T a, T b) { return a + b; }

// В сигнатуре функции
template <typename T>
T half(T x) requires std::floating_point<T> { return x / 2; }

// С лямбдами
auto lambda = []<typename T>(T x) requires std::integral<T> { /* ... */ };



template <typename T>
requires std::copyable<T>
void process(T a) {  }


template <typename T>
T half(T x) requires std::floating_point<T> {  }

template <typename T>
void process(T value) 
    requires std::integral<T> && 
             (sizeof(T) > 4) && 
             requires { typename T::is_special; } 
{
    // Реализация для больших целочисленных типов с флагом is_special
}


template <typename Iter>
void sort(Iter begin, Iter end)
    requires std::random_access_iterator<Iter> &&
             std::totally_ordered<typename Iter::value_type> &&
             requires(Iter it, int n) {
                 { it + n } -> std::same_as<Iter>;
                 { it - n } -> std::convertible_to<Iter>;
                 { it[n] } -> std::same_as<typename Iter::reference>;
             }
{
    // Реализация сортировки
}


template <typename T>
T* allocate()
    requires std::is_default_constructible_v<T> && 
             (alignof(T) > 8) && 
             noexcept(T::operator new(sizeof(T)))
{
    return new T();
}


template <typename T>
void serialize(T obj) 
    requires requires (T x, std::ostream& os) {
        { x.serialize(os) } noexcept -> std::same_as<void>;
        { T::version() } -> std::integral;
    }
{
    os << "Version: " << T::version() << "\n";
    obj.serialize(os);
}


template <typename F>
auto call_and_log(F func) 
    requires requires(F f) {
        { f() } -> std::convertible_to<std::string>;
        requires noexcept(f());
    }
{
    try {
        return func();
    } catch (...) {
        return "error";
    }
}

// Использование
auto result = call_and_log([]() noexcept -> std::string { return "test"; });




template <typename T>
auto transform(T obj) 
    requires requires {
        typename T::result_type;
        { obj.compute() } -> std::same_as<typename T::result_type>;
        requires std::is_integral_v<decltype(obj.compute())>;
        requires (T::factor > 0.5f);
        requires noexcept(obj.reset());
    }
{
    if constexpr (T::factor > 1.0f) {
        return obj.compute() * 2;
    } else {
        return obj.compute();
    }
}





