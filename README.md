*This project has been created as part of the 42 curriculum by csilva-s.*

# Description
Philosophers is a project that covers concepts about concurrency and parallelism between threads and pushes the student to develop and understand a multi-threaded system. Basically, it makes use of an old problem in computing called The Dining Philosophers Problem. This problem gives you a table with some philosophers, who have a routine: eat, sleep and think. In this narrative, each philosopher has only one fork to eat with, but two forks are needed to do so. Given this perspective, let's detail the problem below.

First, each philosopher has a standard amount of time to perform each of the actions in their routine, including the time it takes them to die. When the time to die runs out and the philosopher hasn't managed to eat during that time, they die and consequently the simulation ends. So, as I mentioned before, the philosopher needs two forks to eat, and each one has only a single fork, so it becomes necessary to borrow another fork, necessarily one that is not in use. With these requirements in mind, we can devise the strategy so that everyone manages to eat within the time to die without getting in each other's way when trying to pick up the forks.

# Instructions

To run the project use the command below:
```
git clone <repo-url-here> Philosophers && cd Philosophers/philo/ && make
```
Now just run it (replace the fields with the desired values):
```
./philo <philo_number> <time_to_die> <time_to_eat> <time_to_sleep> <(opitional)number_of_times_each_philosopher_must_eat>
```
[!IMPORTANT]
Running philosophers with more than 200 philosophers may overload your machine.

# Resources

In this section, I will list all the content I used to understand the problem and develop this project.
- [Oceano - Philosophers in C](https://www.youtube.com/watch?v=zOpzGHwJ3MU)
- [Philosophers 42 Explained](https://github.com/DeRuina/philosophers)
- [Unis threads in C](https://www.youtube.com/playlist?list=PLfqABt5AS4FmuQf70psXrsMLEDQXNkLq2)
- [zelhajou - Philosophers](https://github.com/zelhajou/ft_unix_philosophers)
- [Lazy Philosophers Tester](https://github.com/MichelleJiam/LazyPhilosophersTester)
