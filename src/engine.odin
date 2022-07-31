Engine :: struct {
    init(input: ^Input);
    deinit(input: ^Input);
    input: ^Input;
    delta: i64 = 0
    quit: bool = false
    load :: proc() -> void
    render :: proc() -> void
    update :: proc() -> void
}
