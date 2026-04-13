#  10_05_plot.py
import csv
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


def load(path: str):
    xs     = []
    curves = {}

    with open(path, newline="") as f:
        reader = csv.DictReader(f)

        names = [field for field in reader.fieldnames if field != "n"]

        for name in names:
            curves[name] = []

        for row in reader:
            xs.append(int(row["n"]))

            for name in names:
                curves[name].append(int(row[name]))

    return xs, curves

def make_plot(xs, curves, out_path: str):

    fig, (ax_all, ax_good) = plt.subplots(1, 2, figsize=(15, 6))

    bad_functions  = {"PJW", "ELF"}
    line_styles    = ["-", "--", "-.", ":", "-", "--", "-.", ":", "-"]

    for idx, (name, ys) in enumerate(curves.items()):
        ax_all.plot(xs, ys,
                    label     = name,
                    linewidth = 2.0 if name in bad_functions else 1.5,
                    linestyle = line_styles[idx % len(line_styles)])

    ax_all.set_xlabel("Количество хэшируемых строк",  fontsize=11)
    ax_all.set_ylabel("Количество коллизий",          fontsize=11)
    ax_all.set_title ("Все 9 хэш-функций\n"
                       "(строки длины 10 из строчных букв, 32-битное хэш-пространство)",
                       fontsize=11)
    ax_all.legend(fontsize=9)
    ax_all.grid(True, alpha=0.35)
    ax_all.xaxis.set_major_formatter(ticker.FuncFormatter(
        lambda x, _: f"{int(x)//1024}K" if x >= 1024 else str(int(x))
    ))

    for idx, (name, ys) in enumerate(curves.items()):
        if name not in bad_functions:
            ax_good.plot(xs, ys,
                         label     = name,
                         linewidth = 1.5,
                         linestyle = line_styles[idx % len(line_styles)])

    ax_good.set_xlabel("Количество хэшируемых строк",  fontsize=11)
    ax_good.set_ylabel("Количество коллизий",          fontsize=11)
    ax_good.set_title ("Только лучшие функции\n"
                        "(PJW и ELF исключены из графика)",
                        fontsize=11)
    ax_good.legend(fontsize=9)
    ax_good.grid(True, alpha=0.35)
    ax_good.xaxis.set_major_formatter(ticker.FuncFormatter(
        lambda x, _: f"{int(x)//1024}K" if x >= 1024 else str(int(x))
    ))

    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    print(f"Plot saved to {out_path}")
    plt.show()

def summary(xs, curves):

    n_final = xs[-1] + 1 if xs else 0

    print(f"\n{'Function':>8}  {'Collisions':>12}  {'Rank':>6}  {'Notes'}")
    print("-" * 55)

    ranked = sorted(curves.items(), key=lambda kv: kv[1][-1])

    for rank, (name, ys) in enumerate(ranked, 1):
        note = ""
        if name in {"PJW", "ELF"}:
            note = "<- 28-bit effective range"

        print(f"{name:>8}  {ys[-1]:>12}  {rank:>6}  {note}")

    best  = ranked[ 0][0]
    worst = ranked[-1][0]

    print(f"\nBest  function : {best}  ({ranked[0][1][-1]} collisions)")
    print(f"Worst function : {worst} ({ranked[-1][1][-1]} collisions)")

    if n_final:
        theory_32 = n_final ** 2 / (2 * 2**32)
        theory_28 = n_final ** 2 / (2 * 2**28)
        print(f"\nTheoretical expectations for n = {n_final:,}:")
        print(f"  Uniform 32-bit hash  : {theory_32:>10.1f} collisions")
        print(f"  Uniform 28-bit hash  : {theory_28:>10.1f} collisions  (PJW/ELF)")

if __name__ == "__main__":

    xs, curves = load("10_05_data.csv")

    summary(xs, curves)

    make_plot(xs, curves, "10_05_collisions.png")
