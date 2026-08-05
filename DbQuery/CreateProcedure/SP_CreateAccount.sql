
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO

CREATE PROCEDURE SP_CreateAccount 
	@LoginID varchar(255) = 0, 
	@Password varchar(255) = 0
AS
BEGIN
	
	SET NOCOUNT ON;
	SELECT 1 FROM dbo.Account WHERE LoginID = @LoginID
	
	IF @@ROWCOUNT > 0
	BEGIN
		SELECT 1 AS ResultCode, 0 AS AccountID;
	END

	INSERT INTO dbo.Account (LoginID, Password) VALUES (@LoginID, @Password); 
	
	SELECT 0 AS ResultCode;
	SELECT CAST(SCOPE_IDENTITY() AS bigint);
    
END
GO
