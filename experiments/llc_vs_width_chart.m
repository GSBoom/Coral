% L3 cache miss rate vs matrix width — Coral MW4

w = [1024 1536 2048 3072 4096 5120 6144 7168 8192];
miss_rate = [22.5 26.1 29.7 34.3 37.2 38.9 40.5 44.5 45.8];

figure('Position', [100 100 580 420]);

plot(w, miss_rate, '-sk', 'LineWidth', 1.5, 'MarkerSize', 12, ...
         'MarkerFaceColor', 'w');

set(gca, 'FontSize', 17, 'LineWidth', 1.5, 'Box', 'on', ...
         'XTick', [1024 2048 4096 6144 8192]);
xlim([900 8500]);
xlabel('Matrix width {\it m}', 'FontSize', 21);
ylabel('L3 cache miss rate (%)', 'FontSize', 21);
