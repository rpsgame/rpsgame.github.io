#!/usr/bin/env python3
from tinygrad import Tensor, nn

class Model:
  def __init__(self):
    self.l1 = nn.Conv2d(1, 8, kernel_size=(5,5))
    self.l2 = nn.Conv2d(8, 16, kernel_size=(5,5))
    self.l3 = nn.Linear(256, 10)

  def __call__(self, x:Tensor) -> Tensor:
    x = self.l1(x).relu().max_pool2d((2,2))
    x = self.l2(x).relu().max_pool2d((2,2))
    return self.l3(x.flatten(1).dropout(0.5))

model = Model()

from tinygrad.nn.datasets import mnist
X_train, Y_train, X_test, Y_test = mnist()
X_train, X_test = X_train / 255, X_test / 255

optim = nn.optim.Adam(nn.state.get_parameters(model), lr=3e-4) # SGD Adam
batch_size = 256
def step():
  Tensor.training = True  # makes dropout work
  samples = Tensor.randint(batch_size, high=X_train.shape[0])
  X, Y = X_train[samples], Y_train[samples]
  optim.zero_grad()
  loss = model(X).sparse_categorical_crossentropy(Y).backward()
  optim.step()
  return loss

from tinygrad import TinyJit
jit_step = TinyJit(step)

for i in range(20000):
  loss = jit_step()
  if i%batch_size == 0:
    Tensor.training = False
    acc = (model(X_test).argmax(axis=1) == Y_test).mean().item()
    print(f"step {i:4d}, loss {loss.item():.2f}, acc {acc*100.:.2f}%")

from tinygrad.nn.state import safe_save, get_state_dict
state_dict = get_state_dict(model)
safe_save(state_dict, "model.safetensors")
