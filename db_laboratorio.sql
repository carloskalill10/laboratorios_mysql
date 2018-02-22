-- phpMyAdmin SQL Dump
-- version 4.7.0
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: 22-Fev-2018 às 07:08
-- Versão do servidor: 10.1.26-MariaDB
-- PHP Version: 7.1.8

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `db_laboratorio`
--

-- --------------------------------------------------------

--
-- Estrutura da tabela `acesses`
--

CREATE TABLE `acesses` (
  `id` int(11) NOT NULL,
  `dt_acesso` datetime NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL,
  `user_id` int(11) DEFAULT NULL,
  `laboratory_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `acesses`
--

INSERT INTO `acesses` (`id`, `dt_acesso`, `created_at`, `updated_at`, `user_id`, `laboratory_id`) VALUES
(1, '2018-02-15 16:43:13', '2018-02-15 16:43:13', '2018-02-15 16:43:13', 2, 2),
(2, '2018-02-16 01:33:30', '2018-02-16 01:33:30', '2018-02-16 01:33:30', 3, 1),
(3, '2018-02-16 01:37:02', '2018-02-16 01:37:02', '2018-02-16 01:37:02', 3, 1),
(4, '2018-02-16 01:39:00', '2018-02-16 01:39:00', '2018-02-16 01:39:00', 3, 1),
(5, '2018-02-16 01:39:48', '2018-02-16 01:39:48', '2018-02-16 01:39:48', 3, 1),
(6, '2018-02-19 15:13:05', '2018-02-19 15:13:05', '2018-02-19 15:13:05', 4, 1),
(7, '2018-02-19 15:14:26', '2018-02-19 15:14:26', '2018-02-19 15:14:26', 4, 1);

-- --------------------------------------------------------

--
-- Estrutura da tabela `insert_auxes`
--

CREATE TABLE `insert_auxes` (
  `id` int(11) NOT NULL,
  `tag` varchar(255) NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `insert_auxes`
--

INSERT INTO `insert_auxes` (`id`, `tag`, `created_at`, `updated_at`) VALUES
(16, '3AE697E5', '2018-02-16 01:24:41', '2018-02-16 01:24:41'),
(18, '3AE697E5', '2018-02-19 00:25:00', '0000-00-00 00:00:00'),
(19, 'A6F4F313', '2018-02-19 14:48:23', '2018-02-19 14:48:23'),
(20, 'A6F4F313', '2018-02-19 14:49:58', '2018-02-19 14:49:58'),
(21, 'a1d1n1', '2018-02-21 06:45:50', '0000-00-00 00:00:00'),
(22, 'a1d1n1', '2018-02-21 06:47:50', '0000-00-00 00:00:00');

-- --------------------------------------------------------

--
-- Estrutura da tabela `laboratories`
--

CREATE TABLE `laboratories` (
  `id` int(11) NOT NULL,
  `nome` varchar(255) NOT NULL,
  `ativo` tinyint(1) NOT NULL DEFAULT '1',
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `laboratories`
--

INSERT INTO `laboratories` (`id`, `nome`, `ativo`, `created_at`, `updated_at`) VALUES
(1, 'metrologia', 1, '2018-02-15 02:32:30', '2018-02-15 02:32:30'),
(2, 'prog 1', 1, '2018-02-15 10:23:09', '2018-02-16 00:56:50'),
(3, 'prog 2', 1, '2018-02-15 10:27:30', '2018-02-16 00:56:42'),
(4, 'prog 3', 1, '2018-02-16 00:57:01', '2018-02-16 00:57:01'),
(5, 'prog 5', 0, '2018-02-19 14:41:57', '2018-02-19 14:42:34');

-- --------------------------------------------------------

--
-- Estrutura da tabela `reserves`
--

CREATE TABLE `reserves` (
  `id` int(11) NOT NULL,
  `dt_entrada` datetime NOT NULL,
  `dt_saida` datetime NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL,
  `laboratory_id` int(11) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `reserves`
--

INSERT INTO `reserves` (`id`, `dt_entrada`, `dt_saida`, `created_at`, `updated_at`, `laboratory_id`, `user_id`) VALUES
(5, '2018-02-16 00:00:00', '2018-02-16 04:00:00', '2018-02-16 00:40:15', '2018-02-16 00:40:15', 1, NULL),
(6, '2018-02-16 00:00:00', '2018-02-16 04:00:00', '2018-02-16 00:58:15', '2018-02-16 00:58:15', 1, 3),
(7, '2018-02-16 05:00:00', '2018-02-16 06:00:00', '2018-02-16 01:04:38', '2018-02-16 01:04:38', 1, 2),
(8, '2018-02-16 04:00:00', '2018-02-16 05:00:00', '2018-02-16 01:05:09', '2018-02-16 01:05:09', 3, 3),
(9, '2018-02-16 00:00:00', '2018-02-16 02:00:00', '2018-02-16 01:05:56', '2018-02-16 01:05:56', 3, 3),
(10, '2018-02-19 14:00:00', '2018-02-19 16:00:00', '2018-02-19 14:39:04', '2018-02-19 14:39:04', 4, 3),
(11, '2018-02-19 15:00:00', '2018-02-19 16:00:00', '2018-02-19 15:12:05', '2018-02-19 15:12:05', 1, 4);

-- --------------------------------------------------------

--
-- Estrutura da tabela `users`
--

CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `nome` varchar(255) NOT NULL,
  `tag` varchar(255) NOT NULL,
  `ativo` tinyint(1) NOT NULL DEFAULT '1',
  `tipo` int(11) NOT NULL DEFAULT '0',
  `funcao` int(11) NOT NULL DEFAULT '0',
  `login` varchar(255) NOT NULL,
  `senha` varchar(255) NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `users`
--

INSERT INTO `users` (`id`, `nome`, `tag`, `ativo`, `tipo`, `funcao`, `login`, `senha`, `created_at`, `updated_at`) VALUES
(2, 'adrisson', 'BD2798E5', 1, 1, 1, 'adrisson', '123', '2018-02-15 13:13:33', '2018-02-15 13:13:33'),
(3, 'carlos kenneton rocha de almeida', '3AE697E5', 1, 1, 0, 'carlos', '123', '2018-02-16 00:54:49', '2018-02-16 00:55:07'),
(4, 'Hillermann lima osmidio', 'A6F4F313', 1, 1, 0, 'hill', '123', '2018-02-19 14:51:42', '2018-02-19 14:55:13'),
(5, 'carlos', 'a1d1n1', 1, 1, 0, '123', '$2a$10$.wRImghD59ynLwpLTuTg/ep7Y3nKmLXWldId7MClwbz4q3i4SsH7G', '2018-02-21 06:48:01', '2018-02-21 06:48:01');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `acesses`
--
ALTER TABLE `acesses`
  ADD PRIMARY KEY (`id`),
  ADD KEY `user_id` (`user_id`),
  ADD KEY `laboratory_id` (`laboratory_id`);

--
-- Indexes for table `insert_auxes`
--
ALTER TABLE `insert_auxes`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `laboratories`
--
ALTER TABLE `laboratories`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `reserves`
--
ALTER TABLE `reserves`
  ADD PRIMARY KEY (`id`),
  ADD KEY `laboratory_id` (`laboratory_id`),
  ADD KEY `user_id` (`user_id`);

--
-- Indexes for table `users`
--
ALTER TABLE `users`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `acesses`
--
ALTER TABLE `acesses`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=8;
--
-- AUTO_INCREMENT for table `insert_auxes`
--
ALTER TABLE `insert_auxes`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=23;
--
-- AUTO_INCREMENT for table `laboratories`
--
ALTER TABLE `laboratories`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
--
-- AUTO_INCREMENT for table `reserves`
--
ALTER TABLE `reserves`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;
--
-- AUTO_INCREMENT for table `users`
--
ALTER TABLE `users`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
--
-- Constraints for dumped tables
--

--
-- Limitadores para a tabela `acesses`
--
ALTER TABLE `acesses`
  ADD CONSTRAINT `acesses_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `acesses_ibfk_2` FOREIGN KEY (`laboratory_id`) REFERENCES `laboratories` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Limitadores para a tabela `reserves`
--
ALTER TABLE `reserves`
  ADD CONSTRAINT `reserves_ibfk_1` FOREIGN KEY (`laboratory_id`) REFERENCES `laboratories` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  ADD CONSTRAINT `reserves_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
