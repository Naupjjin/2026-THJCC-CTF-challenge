#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define CONTRACT_SIGN   0x16C0  
#define PURE_SOUL_GEM   0x16C1 
#define ATTACK          0x16C2

#define GEM_SIZE    0x400

struct contract {
    char name[0x10];
    char wish[0x30];
};

struct PureGem {
    char essence[GEM_SIZE];
};

struct pure_size {
    int size;
};

struct attack_t {
    unsigned int Dance_of_Magic_Bullets;
    unsigned int Infinite_Magical_Aria; 
    unsigned int Tiro_Finale;
    unsigned int Bombardamento;
    unsigned int Legare_Vasta_Area;
};

struct witch_info {
    unsigned int hp;
};

static DEFINE_MUTEX(LOCK);

static int griefseed_num = 0;
static struct PureGem *puregem = NULL;

static struct witch_info witch = {
    .hp = 9627,
};

static long contract_sign (struct contract contract_arg) {
    if (puregem != NULL) {
        return -1;
    }

    puregem = kzalloc(sizeof(struct PureGem), GFP_KERNEL);
    if (!puregem) {
        pr_info("[QB] sign error\n");
        return -1;
    }

    pr_info("[QB] Name: %.16s, Wish: %.30s\n", contract_arg.name, contract_arg.wish);
    pr_info("[QB] puregem pointer: %px\n", puregem);
    return 0;
}


static long pure_soul_gem (struct pure_size pure_arg) {
    if (!puregem) {
        pr_info("No contract signed!\n");
        return -1;
    }

    if (pure_arg.size < 0 || pure_arg.size > GEM_SIZE) {
        pr_info("size error\n");
        return -1;
    }

    for (int i = 0; i <= pure_arg.size; i++) {
        puregem->essence[i] = '\0';
    }

    pr_info("Pure Success\n");
    return 0;
}


static long attack_witch (struct attack_t attack_arg) {
    if (puregem == NULL) {
        pr_alert("もう何も怖くない\n");
        BUG();
    }

    unsigned int attack_result = 0;

    attack_result += (unsigned long)attack_arg.Dance_of_Magic_Bullets * 222;
    attack_result += (unsigned long)attack_arg.Infinite_Magical_Aria * 103;
    attack_result += (unsigned long)attack_arg.Tiro_Finale * 605;
    attack_result += (unsigned long)attack_arg.Bombardamento * 920;
    attack_result += (unsigned long)attack_arg.Legare_Vasta_Area * 509;

    pr_info("[QB] Attack: %u\n", attack_result);

    if (witch.hp - attack_result == 0) {
        griefseed_num++;
        pr_info("Success attack\n");        
    } else {
        pr_alert("もう何も怖くない\n");
        BUG();
    }

    return 0;

}

static long qb_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    long ret;

    mutex_lock(&LOCK);

    switch (cmd) {
        case CONTRACT_SIGN: {
            struct contract contract_arg;
            if (copy_from_user(&contract_arg, (void *)arg, sizeof(contract_arg)) != 0) {
                return -1;
            }

            ret = contract_sign(contract_arg);
            break;
        }
        case PURE_SOUL_GEM: {
            struct pure_size pure_arg;
            if (copy_from_user(&pure_arg, (void *)arg, sizeof(pure_arg)) != 0) {
                return -1;
            }

            ret = pure_soul_gem(pure_arg);
            break;
        }
        case ATTACK: {
            struct attack_t attack_arg;
            if (copy_from_user(&attack_arg, (void *)arg, sizeof(attack_arg)) != 0) {
                return -1;
            }
            ret = attack_witch(attack_arg);
            break;
        }
        default:
            ret = -1;
            break;
    }

    mutex_unlock(&LOCK);
    return ret;
}

static struct file_operations qb_fops = {
    .unlocked_ioctl = qb_ioctl,
};

static struct miscdevice qb = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "QB",
    .fops = &qb_fops,
    .mode = 0666,
};

static int __init qb_init(void) {
    if (misc_register(&qb) != 0)
        return -1;

    pr_info("[QB] 僕と契約して魔法少女になってよ！\n");
    return 0;
}

static void __exit qb_exit(void) {
    misc_deregister(&qb);
    mutex_destroy(&LOCK);
    pr_info("[QB] Bye~.\n");
}

module_init(qb_init);
module_exit(qb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("naup96321");
MODULE_DESCRIPTION("OuO");