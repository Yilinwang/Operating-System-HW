#include <linux/list.h>
#include <linux/sched.h>
/*
 * SCHED_WEIGHTED_RR scheduling class. Implements a round robin scheduler with weight
 * priority mechanism.
 */

/*
 * Update the current task's runtime statistics. Skip current tasks that
 * are not in our scheduling class.
 */
static void update_curr_lifo(struct rq *rq)
{
	struct task_struct *curr = rq->curr;
	u64 delta_exec;

	if (!task_has_lifo_policy(curr))
		return;

	delta_exec = rq->clock - curr->se.exec_start;
	if (unlikely((s64)delta_exec < 0))
		delta_exec = 0;

	schedstat_set(curr->se.exec_max, max(curr->se.exec_max, delta_exec));

	curr->se.sum_exec_runtime += delta_exec;
	curr->se.exec_start = rq->clock;
	cpuacct_charge(curr, delta_exec);
}

/*
 * Adding/removing a task to/from a priority array:
 */
static void enqueue_task_lifo(struct rq *rq, struct task_struct *p, int wakeup, bool b)
{
	list_add(&p->lifo_list_item, &rq->lifo.queue);
	rq->lifo.nr_running++;
}

static void dequeue_task_lifo(struct rq *rq, struct task_struct *p, int sleep)
{
	update_curr_lifo(rq);
	list_del(&p->lifo_list_item);
	rq->lifo.nr_running--;
}

/*
 * Put task to the end of the run list without the overhead of dequeue
 * followed by enqueue.
 */
static void requeue_task_lifo(struct rq *rq, struct task_struct *p)
{
	list_move_tail(&p->lifo_list_item, &rq->lifo.queue);
}

/*
 * current process is relinquishing control of the CPU
 */
static void
yield_task_lifo(struct rq *rq)
{
	//list_move_tail(&rq->curr->lifo_list_item, &rq->lifo.queue);
}

/*
 * Preempt the current task with a newly woken task if needed:
 * int wakeflags added to match function signature of other schedulers
 */
static void check_preempt_curr_lifo(struct rq *rq, struct task_struct *p, int wakeflags)
{
}

/*
 * select the next task to run
 */
static struct task_struct *pick_next_task_lifo(struct rq *rq)
{
	return (rq->lifo.nr_running > 0)? list_first_entry(&rq->lifo.queue, struct task_struct, lifo_list_item): NULL;
}

static void put_prev_task_lifo(struct rq *rq, struct task_struct *p)
{
	update_curr_lifo(rq);
	p->se.exec_start = 0;
}

#ifdef CONFIG_SMP
/*
 * Load-balancing iterator. Note: while the runqueue stays locked
 * during the whole iteration, the current task might be
 * dequeued so the iterator has to be dequeue-safe. Here we
 * achieve that by always pre-iterating before returning
 * the current task:
 */
static struct task_struct *load_balance_start_lifo(void *arg)
{	
	struct rq *rq = arg;
	struct list_head *head, *curr;
	struct task_struct *p;

	head = &rq->lifo.queue;
	curr = head->prev;

	p = list_entry(curr, struct task_struct, lifo_list_item);

	curr = curr->prev;

	rq->lifo.lifo_load_balance_head = head;
	rq->lifo.lifo_load_balance_curr = curr;

	return p;
}

static struct task_struct *load_balance_next_lifo(void *arg)
{
	struct rq *rq = arg;
	struct list_head *curr;
	struct task_struct *p;

	curr = rq->lifo.lifo_load_balance_curr;

	p = list_entry(curr, struct task_struct, lifo_list_item);
	curr = curr->prev;
	rq->lifo.lifo_load_balance_curr = curr;

	return p;
}

static unsigned long
load_balance_lifo(struct rq *this_rq, int this_cpu, struct rq *busiest,
		unsigned long max_load_move,
		struct sched_domain *sd, enum cpu_idle_type idle,
		int *all_pinned, int *this_best_prio)
{
	struct rq_iterator lifo_rq_iterator;

	lifo_rq_iterator.start = load_balance_start_lifo;
	lifo_rq_iterator.next = load_balance_next_lifo;
	/* pass 'busiest' rq argument into
	 * load_balance_[start|next]_lifo iterators
	 */
	lifo_rq_iterator.arg = busiest;

	return balance_tasks(this_rq, this_cpu, busiest, max_load_move, sd,
			     idle, all_pinned, this_best_prio, &lifo_rq_iterator);
}

static int
move_one_task_lifo(struct rq *this_rq, int this_cpu, struct rq *busiest,
		 struct sched_domain *sd, enum cpu_idle_type idle)
{
	struct rq_iterator lifo_rq_iterator;

	lifo_rq_iterator.start = load_balance_start_lifo;
	lifo_rq_iterator.next = load_balance_next_lifo;
	lifo_rq_iterator.arg = busiest;

	return iter_move_one_task(this_rq, this_cpu, busiest, sd, idle,
				  &lifo_rq_iterator);
}
#endif

/*
 * task_tick_lifo is invoked on each scheduler timer tick.
 */
static void task_tick_lifo(struct rq *rq, struct task_struct *p,int queued)
{
}

/*
 * scheduling policy has changed -- update the current task's scheduling
 * statistics
 */
static void set_curr_task_lifo(struct rq *rq)
{
	struct task_struct *p = rq->curr;
	p->se.exec_start = rq->clock;
}

/*
 * We switched to the sched_lifo class.
 */
static void switched_to_lifo(struct rq *rq, struct task_struct *p,
			     int running)
{
	/*
	 * Kick off the schedule if running, otherwise just see
	 * if we can still preempt the current task.
	 */
	if (running)
		resched_task(rq->curr);
	else
		check_preempt_curr(rq, p, 0);
}

static int
select_task_rq_lifo(struct rq *rq, struct task_struct *p, int sd_flag, int flags)
{
	if (sd_flag != SD_BALANCE_WAKE)
		return smp_processor_id();

	return task_cpu(p);
}

const struct sched_class lifo_sched_class = {
	.next			= &idle_sched_class,
	.enqueue_task		= enqueue_task_lifo,
	.dequeue_task		= dequeue_task_lifo,
	.yield_task		= yield_task_lifo,

	.check_preempt_curr	= check_preempt_curr_lifo,

	.pick_next_task		= pick_next_task_lifo,
	.put_prev_task		= put_prev_task_lifo,

#ifdef CONFIG_SMP
	.load_balance		= load_balance_lifo,
	.move_one_task		= move_one_task_lifo,
#endif

	.switched_to  = switched_to_lifo,
	.select_task_rq = select_task_rq_lifo,

	.set_curr_task          = set_curr_task_lifo,
	.task_tick		= (void *)task_tick_lifo,
};
