import matplotlib.pyplot as plt

episodes = []
winRate = []
drawRate = []
lossRate = []

with open("winData_100k.txt", "r") as file:
    for line in file:
        ep, win, loss, draw = line.split()
        episodes.append(int(ep))
        winRate.append(float(win))
        lossRate.append(float(loss))
        drawRate.append(float(draw))

plt.plot(episodes, winRate, label = 'Win Rate')
plt.plot(episodes, lossRate, label = 'Loss Rate')
plt.plot(episodes, drawRate, label = 'Draw Rate')

plt.xlabel('Episodes')
plt.ylabel('Rate(%)')
plt.title('Learning Curve of RL Model with Epsilon Greedy')
plt.legend()

plt.show()