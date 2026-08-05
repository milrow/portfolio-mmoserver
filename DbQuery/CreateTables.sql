CREATE TABLE dbo.Account (
	AccountID bigint IDENTITY(1,1) PRIMARY KEY,
	LoginID nvarchar(255) NOT NULL UNIQUE,
	Password nvarchar(255) NOT NULL
);
