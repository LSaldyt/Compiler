
class A():
    def __init__(self):
        print('Created Object')

    def foo(self, x):
        print(x*x)

if __name__ == '__main__':
    a = A()
    a.foo(2)
