import json
import argparse
import matplotlib.pyplot as plt


def modulator_from_mode(mode: str) -> str:
  match mode:
    case '1':
      return 'QPSK'
    case '2':
      return '16QAM'
    case '3':
      return '64QAM'
  return 'Unsupported Modulation Mode'


def plot_chart(data):  
  plt.figure('BER/Variance Chart', figsize=(10, 6))

  for mode, variance_map in data.items():
    variances = sorted(variance_map.keys())

    bers = []
    for variance in variances:
      bers.append(variance_map[variance])

    plt.plot(
      variances,
      bers,
      marker="o",
      label=modulator_from_mode(mode)
    )
    
  plt.xlabel("Variance")
  plt.ylabel("BER")
  plt.title("BER vs Variance for Modulation Modes")
  plt.yscale("log")

  plt.grid(True, which="both", linestyle="--", alpha=0.5)
  plt.legend()
  plt.tight_layout()

  plt.show()


def parse_args():
  parser = argparse.ArgumentParser(
    description="Plot BER vs Variance for modulation modes"
  )

  parser.add_argument(
    "--json",
    required=True,
    help="Path to JSON containing data for chart."
  )

  return parser.parse_args()


def main():
  args = parse_args()

  with open(args.json, "r", encoding="utf-8") as f:
    data = json.load(f)
  
  if not data:
    print('Invalid path for --json argument.')
  else:
    plot_chart(data)


if __name__ == '__main__':
    main()
