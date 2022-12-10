#%%
xs = []
ys = []
with open("data.txt", "r") as file:
    for line in file:
        x_raw, y_raw = line.split(":")
        print(line.split(":"))
        
        xs.append(int(x_raw))
        ys.append(int(y_raw))
# %%


import matplotlib.pyplot as plt
from scipy import interpolate

f = interpolate.interp1d(xs, ys, fill_value='extrapolate')

plt.yscale("log")
# plt.plot(xs, ys)
plt.scatter(xs, ys)

xs_new = [i for i in range(1, 29) if i not in xs]
ys_new = f(xs_new)

plt.scatter(xs_new, ys_new, c="r")


# %%

ys_new[-1]
# %%
